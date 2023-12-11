#include <deque>
#include <list>

#include "gtest/gtest.h"
#include "gtest/gtest_prod.h"

#include "pevents.h"

TEST(Event_TST, SetEvent) {
    Event* ev = new Event();
    std::shared_ptr<Event> evSmartPtr(ev);
    ASSERT_EQ(ev->isSetted, false);
    SetEvent(evSmartPtr);
    EXPECT_EQ(ev->isSetted, true);
}

TEST(Event_TST, WaitEvents) {
    Event* ev = new Event();
    std::shared_ptr<Event> evSmartPtr(ev);
    SetEvent(evSmartPtr);
    std::list<std::shared_ptr<Event>> listEv;
    listEv.push_back(evSmartPtr);
    waitEvents(listEv);
    EXPECT_EQ(ev->waitVector.size(), 0);
}

TEST(Event_TST, WaitEvents_Delay_1Sec) {
    auto Waiting = [](std::shared_ptr<Event> evSmartPtr) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        SetEvent(evSmartPtr);
    };

    Event* ev = new Event();
    std::shared_ptr<Event> evSmartPtr(ev);
    EXPECT_EQ(ev->isSetted, false);
    std::thread waitThread(Waiting, evSmartPtr);
    waitThread.detach();
    std::deque<std::shared_ptr<Event>> dequeEv;
    dequeEv.push_back(evSmartPtr);
    waitEvents(dequeEv);
    EXPECT_EQ(ev->isSetted, true);
}

TEST(Event_TST, Buf_WaitEvents_Delay_4Sec) {
    auto Waiting = [](std::shared_ptr<Event> evSmartPtr) {
        std::this_thread::sleep_for(std::chrono::seconds(4));
        SetEvent(evSmartPtr);
    };

    const int bufSize = 100;
    std::vector<Event*> bufEvents;
    for (int i = 0; i < bufSize; i++) {
        Event* ev = new Event();
        EXPECT_EQ(ev->isSetted, false);
        bufEvents.push_back(ev);
    }

    std::list<std::shared_ptr<Event>> bufEventsPtr;
    for (auto ev = bufEvents.begin(); ev != bufEvents.end(); ev++)
        bufEventsPtr.push_back(std::shared_ptr<Event>(*ev));

    for (auto evSmartPtr = bufEventsPtr.begin(); evSmartPtr != bufEventsPtr.end();
         evSmartPtr++) {
        std::thread waitThread(Waiting, *evSmartPtr);
        waitThread.detach();
    }

    waitEvents(bufEventsPtr);
    for (auto ev = bufEvents.begin(); ev != bufEvents.end(); ev++)
        EXPECT_EQ((*ev)->isSetted, true);
}
