#pragma once

#include <atomic>
#include <memory>
#include <random>

#include "Log.h"
#include "Server.h"
#include "pevents.h"

class Client {
    class ReqInfo {
        friend Client;
        std::atomic<int> sendReq;
        std::atomic<int> finishedReq;
        std::atomic<int> notFinishedReq;
        int64_t maxDalay{0};
        int64_t ClientID{0};
        std::atomic<int> reqID;

      public:
        ReqInfo() : sendReq{0}, finishedReq{0}, notFinishedReq{0}, reqID{0} {}
    };

    std::mutex compareM;
    std::mutex randM;
    std::mt19937 generator;
    std::atomic<bool> isRun;
    std::vector<std::shared_ptr<Event>> eventsClients;
    std::shared_ptr<Server> server;
    int64_t sec;
    int64_t getRand(int64_t max);
    void requestTask(ReqInfo* reqInfo, std::shared_ptr<Event> reqEvent);
    void runClient(int clientId, std::shared_ptr<Event> pClientEvent);

  public:
    void setServer(std::shared_ptr<Server> server);
    void runPoolThreads(int maxClientThreads);
    void setWorkTime(int sec);
};
