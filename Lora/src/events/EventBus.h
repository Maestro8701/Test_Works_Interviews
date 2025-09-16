//
// Created by never on 20.05.2025.
//

#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <mutex>
#include <queue>

#include "freertos/FreeRTOS.h"
#include <freertos/semphr.h>
#include "EventListener.h"

static constexpr size_t MAX_LISTENERS = 20;
static constexpr size_t EVENT_QUEUE_LEN = 20;

class EventBus {
public:
    void init();
    bool subscribe(EventListener* listener);
    void emit(const Event& event);
    void emit(std::shared_ptr<Event> event);
    size_t getSubscribersCount() const;
private:
    TaskHandle_t _dispatcherTaskHandle = nullptr;
    static void dispatcherTaskEntry(void* parameter);
    void dispatcherTask();

    std::mutex _queueMutex;
    std::queue<std::shared_ptr<Event>> _eventQueue;

    std::mutex _subscriberMutex;
    std::vector<EventListener*> _subscribers;
};


#endif //EVENTBUS_H
