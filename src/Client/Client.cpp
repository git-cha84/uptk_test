

#include "Client.h"

void Client::requestTask(ReqInfo* reqInfo, std::shared_ptr<Event> reqEvent) {
    debugLog("[%s]:Client:%04d. Begin request:%d. ",
             __FUNCTION__,
             reqInfo->ClientID,
             reqInfo->reqID.load());
    reqInfo->sendReq.fetch_add(1);
    auto startT = clock();
    try {
        std::promise<uint64_t> promise;
        std::future<uint64_t> future = promise.get_future();
        uint64_t inPut = GlobalConf::MAX_FACTORIAL;
        server->runAsioTask(inPut, std::move(promise));
        future.wait();
        future.get();
        reqInfo->finishedReq.fetch_add(1);

        {
            auto delay = clock() - startT;
            const std::lock_guard<std::mutex> lock(compareM);
            if (delay > reqInfo->maxDalay)
                reqInfo->maxDalay = delay;
        }

    } catch (...) {
        reqInfo->notFinishedReq.fetch_add(1);
        debugLog("[%s]:Client:%04d. Exception request:%d",
                 __FUNCTION__,
                 reqInfo->ClientID,
                 reqInfo->reqID.load());
    }

    debugLog("[%s]:Client:%04d. Finish request:%d",
             __FUNCTION__,
             reqInfo->ClientID,
             reqInfo->reqID.load());
    reqInfo->reqID.fetch_add(1);
    SetEvent(std::move(reqEvent));
}

void Client::setServer(std::shared_ptr<Server> server) {
    this->server = std::move(server);
}

void Client::runPoolThreads(int maxClientThreads) {
    debugLog("[%s]: Begin", __FUNCTION__);
    auto startT = clock();
    isRun.store(true);
    int creatClient = 0;
    for (int i = 0; i < maxClientThreads; i++) {
        auto pClientEvent = std::make_shared<Event>();
        eventsClients.push_back(pClientEvent);
        try {
            std::thread clientThr(&Client::runClient, this, i, pClientEvent);
            clientThr.detach();
            creatClient++;
        } catch (...) {
            SetEvent(pClientEvent);
            i = maxClientThreads;
        }
    }

    std::this_thread::sleep_for(std::chrono::seconds(sec));
    isRun.store(false);
    waitEvents(eventsClients);
    logs::Write("WAS CREATED CLIENT:%d", creatClient);
    logs::Write("ALL CLIENTS IS FINISHED");
    server->printStat();

    auto microsec = (clock() - startT) / ((CLOCKS_PER_SEC / 1000000));    // NOLINT
    logs::Write("WORK TIME:%lu microseconds", microsec);
}

void Client::runClient(int clientId, std::shared_ptr<Event> pClientEvent) {
    logs::Write("BEGIN CLIENT:%04d", clientId);
    std::vector<std::shared_ptr<Event>> eventsReq;
    ReqInfo reqInfo;
    reqInfo.ClientID = clientId;
    while (isRun.load()) {
        auto wait = getRand(GlobalConf::WAITING_NEXT_REQ_TO_SERV);
        std::this_thread::sleep_for(std::chrono::milliseconds(wait));
        auto pReqEvent = std::make_shared<Event>();
        eventsReq.push_back(pReqEvent);
        try {
            std::thread reqThr(&Client::requestTask, this, &reqInfo, pReqEvent);
            reqThr.detach();
        } catch (...) {
            SetEvent(pReqEvent);
            debugLog("[%s]:Client:%04d. Can't create request",
                     __FUNCTION__,
                     reqInfo.ClientID,
                     reqInfo.reqID.load());
        }
    }

    waitEvents(eventsReq);

    auto microsec = reqInfo.maxDalay / ((CLOCKS_PER_SEC / 1000000));    // NOLINT
    logs::Write("FINISHED CLIENT:%04d. \nClient Statistic: Send req:%d; Finish req:%d; "
                "NOT Finish req:%d; Max req delay:%llu microsec\n",
                clientId,
                reqInfo.sendReq.load(),
                reqInfo.finishedReq.load(),
                reqInfo.notFinishedReq.load(),
                microsec);

    SetEvent(std::move(pClientEvent));
}

void Client::setWorkTime(int sec) {
    this->sec = sec;
}

int64_t Client::getRand(int64_t max) {
    const std::lock_guard<std::mutex> lock(randM);
    int64_t res = generator() % max;
    return res;
}
