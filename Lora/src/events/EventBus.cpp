//
// Created by never on 20.05.2025.
//

#include "EventBus.h"

#include <cstring>

#include "DebugLog.h"

void EventBus::init() {
    xTaskCreate(
        dispatcherTaskEntry,
        "EventBusDispatcher",
        8192,
        this,
        2,
        &_dispatcherTaskHandle);
}

bool EventBus::subscribe(EventListener *listener) {
   std::lock_guard lock(_subscriberMutex);
    _subscribers.push_back(listener);

    return true;
}

void EventBus::emit(const Event &event) {
    emit(std::make_shared<Event>(event));
}

void EventBus::emit(std::shared_ptr<Event> event) {
    {
        std::lock_guard lock(_queueMutex);
        _eventQueue.push(std::move(event));
    }

    if (_dispatcherTaskHandle) {
        xTaskNotifyGive(_dispatcherTaskHandle);
    }
}


size_t EventBus::getSubscribersCount() const {
    return _subscribers.size();
}

void EventBus::dispatcherTaskEntry(void *parameter) {
    static_cast<EventBus*>(parameter)->dispatcherTask();
}

void EventBus::dispatcherTask() {
    Event receivedEvent{};

    while (true) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        std::queue<std::shared_ptr<Event>> localQueue;

        {
            std::lock_guard lock(_queueMutex);
            std::swap(localQueue, _eventQueue);
        }

        while (!localQueue.empty()) {
            std::shared_ptr<Event> evt = localQueue.front();
            localQueue.pop();

            std::lock_guard<std::mutex> lock(_subscriberMutex);
            for (auto* listener : _subscribers) {
                if (listener) {
                    listener->onEvent(*evt);
                }
            }
        }
    }
}
