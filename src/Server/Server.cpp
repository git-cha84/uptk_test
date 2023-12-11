#include "Server.h"

void Server::printStat() {
    logs::Write(
        "SERVER Statistic: \nbegined req:%d, finished req:%d, NOT finished req:%d\n",
        beginReq.load(),
        finishReq.load(),
        notFinishReq.load());
}

Server::Server() : beginReq{0}, finishReq{0}, notFinishReq{0} {
    logs::Write("SERVER Statistic:\n");
    debugLog("Server Constructor %s \n", __FUNCTION__);
}

bool Server::runHighLoadWork(uint64_t fact, std::promise<uint64_t> result) {
    beginReq.fetch_add(1);

    auto prob = generator() % GlobalConf::PROBABILITY_THROW;
    if (prob < 25) {    // NOLINT
        notFinishReq.fetch_add(1);
        throw "throw exception";
    }

    auto factorial = 1;
    for (; fact > 1; fact--) {
        factorial = factorial * fact;
        factorial--;
        std::string str = std::to_string(factorial);
        factorial = std::stoul(str);
    }

    result.set_value(factorial);
    finishReq.fetch_add(1);

    return true;
}

void Server::runAsioTask(uint64_t fact, std::promise<uint64_t> result) {
    try {
        auto res = std::async(
            std::launch::async, &Server::runHighLoadWork, this, fact, std::move(result));
    } catch (...) {
        debugLog("[%s]:Error creation ASIO request. ", __FUNCTION__);
        throw "throw ASIO";
    }
}
