#include "pevents.h"

uint64_t Event::count = 0;
uint64_t WaitStatus::count = 0;

Event::Event() {
    const std::lock_guard<std::mutex> lock(constuctM);
    Event::count++;
    id = Event::count;
}

WaitStatus::WaitStatus() {
    const std::lock_guard<std::mutex> lock(constuctM);
    WaitStatus::count++;
    id = WaitStatus::count;
}

bool SetEvent(const std::shared_ptr<Event> event) {
    const std::lock_guard<std::mutex> lock(event->Mutex);
    if (event->isSetted) {
        debugLog("%[s]: Event id:%04lu is already setted", __FUNCTION__, event->id);
        return false;
    }

    event->isSetted = true;
    for (auto const& waitStat : event->waitVector) {
        const std::lock_guard<std::mutex> lock(waitStat->Mutex);
        waitStat->waitEventCount.fetch_sub(1);
        debugLog("[%s]: Event id:%04lu; WaitStat id:%04lu; waitEventCount=%d ",
                 __FUNCTION__,
                 event->id,
                 waitStat->id,
                 waitStat->waitEventCount.load());
        if (waitStat->waitEventCount.load() == 0)
            waitStat->CV.notify_one();
    }
    return true;
}
