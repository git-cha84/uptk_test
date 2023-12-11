#pragma once

#include <atomic>
#include <future>
#include <memory>
#include <random>

#include "Log.h"
#include "config.h"

class Server {
    std::atomic<int> beginReq;
    std::atomic<int> finishReq;
    std::atomic<int> notFinishReq;
    std::mt19937 generator;
    bool runHighLoadWork(uint64_t fact, std::promise<uint64_t> result);

  public:
    void runAsioTask(uint64_t fact, std::promise<uint64_t> result);
    void printStat();
    Server();
};
