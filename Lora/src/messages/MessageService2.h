//
// Created by never on 21.05.2025.
//

#ifndef MESSAGESERVICE2_H
#define MESSAGESERVICE2_H
#include "Message.h"
#include "Module.h"
#include "modules/SX127x/SX1276.h"
#include "protocol/Frame.h"

class MessageService2 {
public:
    explicit MessageService2(SX1276& lora) : _lora(lora) {}

    [[nodiscard]] uint16_t send(const Message& message) const;
    [[nodiscard]] uint16_t sendFrame(const Frame& frame) const;

    Message createAckForMessage(const Message& message);
    Message createAckForMessage(const PacketHeader& header);
    Message createHelloMessage();
private:
    SX1276& _lora;
};

#endif //MESSAGESERVICE2_H
