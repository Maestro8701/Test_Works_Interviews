//
// Created by never on 20.05.2025.
//

#ifndef EVENTTYPE_H
#define EVENTTYPE_H
#include <cstdint>
#include <memory>

enum class EventType : uint8_t {
    RAW_PACKET,
    MESSAGE_READY,
    FOR_RELAY,
    ACK_TIMEOUT,
    ACK_RECEIVED
};

struct Event {
    EventType type;
    std::shared_ptr<void> data;

    template<typename T>
    static Event make(EventType type, std::shared_ptr<T> ptr) {
        return Event {type, ptr};
    }

    template<typename T>
    std::shared_ptr<T> get() const {
        return std::static_pointer_cast<T>(data);
    }
};

#endif //EVENTTYPE_H
