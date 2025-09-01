//
// Created by never on 21.05.2025.
//

#ifndef PACKETPROCESSOR_H
#define PACKETPROCESSOR_H
#include "events/EventBus.h"

class PacketProcessor : public EventListener {
public:
    explicit PacketProcessor(EventBus& eventBus): _eventBus(eventBus) {
    }

    void onEvent(const Event& event) override;
private:
    EventBus& _eventBus;
};

#endif //PACKETPROCESSOR_H
