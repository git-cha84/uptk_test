#pragma once

#include <algorithm>
#include <future>
#include <vector>

#include "gtest/gtest_prod.h"

#include "Log.h"
#include "config.h"

class Event;

class WaitStatus {
    friend bool SetEvent(std::shared_ptr<Event> event);
    template <typename T>
    friend void waitEvents(T& eventsStore);
    std::mutex Mutex;
    std::condition_variable CV;
    std::atomic<int> waitEventCount;
    std::unique_lock<std::mutex> locker;
    std::mutex lockerMutex;
    std::mutex constuctM;
    uint64_t id;
    static uint64_t count;

  public:
    WaitStatus();
};

class Event {
    FRIEND_TEST(Event_TST, SetEvent);
    FRIEND_TEST(Event_TST, WaitEvents);
    FRIEND_TEST(Event_TST, WaitEvents_Delay_1Sec);
    FRIEND_TEST(Event_TST, Buf_WaitEvents_Delay_4Sec);
    friend bool SetEvent(std::shared_ptr<Event> event);
    template <typename T>
    friend void waitEvents(T& eventsStore);
    std::mutex Mutex;
    std::condition_variable CV;
    bool isSetted = false;
    std::vector<std::shared_ptr<WaitStatus>> waitVector;
    std::mutex constuctM;
    uint64_t id;
    static uint64_t count;

  public:
    Event();
};

template <typename T>
void waitEvents(T& eventsStore) {
    debugLog("[%s]: Enter to waitEvents ", __FUNCTION__);
    auto waitStatus = std::make_shared<WaitStatus>();
    int notSettedEvents = 0;

    {
        const std::lock_guard<std::mutex> lock(waitStatus->Mutex);
        for (auto ev = eventsStore.begin(); ev != eventsStore.end(); ev++) {
            const std::lock_guard<std::mutex> lock((*ev)->Mutex);
            if (!(*ev)->isSetted) {
                debugLog(
                    "[%s]: Event id:%04lu now is NOT setted", __FUNCTION__, (*ev)->id);
                (*ev)->waitVector.push_back(waitStatus);
                notSettedEvents++;
            }
        }
        debugLog("[%s]: For waitStatus id:%04lu founded %04d NOT setted events",
                 __FUNCTION__,
                 waitStatus->id,
                 notSettedEvents);
        waitStatus->waitEventCount.store(notSettedEvents);
    }

    if (notSettedEvents) {
        std::unique_lock<std::mutex> lock(waitStatus->lockerMutex);
        waitStatus->CV.wait(lock);
    }
    debugLog(
        "[%s]: For waitStatus id:%04lu happened event", __FUNCTION__, waitStatus->id);

    for (auto ev = eventsStore.begin(); ev != eventsStore.end(); ev++) {
        auto begin = (*ev)->waitVector.begin();
        auto end = (*ev)->waitVector.end();
        std::remove(begin, end, waitStatus);
    }
}
