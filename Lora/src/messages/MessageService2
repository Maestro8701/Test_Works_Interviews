//
// Created by never on 21.05.2025.
//

#include "MessageService2.h"

#include "Utils.h"
#include "protocol/MessageCodec.h"
#include "protocol/PacketHeader.h"

uint16_t MessageService2::send(const Message& message) const {
    static uint8_t buffer[MAX_PACKET_SIZE];

    const size_t packetSize = MessageCodec::encode(message, buffer, sizeof(buffer));

    if (packetSize == 0) {
        return RADIOLIB_ERR_PACKET_TOO_LONG;
    }

    return _lora.transmit(buffer, packetSize);
}

uint16_t MessageService2::sendFrame(const Frame& frame) const {
    static uint8_t buffer[MAX_PACKET_SIZE];

    const size_t packetSize = MessageCodec::encodeFrame(frame, buffer, sizeof(buffer));

    if (packetSize == 0) {
        return RADIOLIB_ERR_PACKET_TOO_LONG;
    }

    return _lora.transmit(buffer, packetSize);
}

Message MessageService2::createAckForMessage(const Message& message) {
    Message ackMessage{};

    ackMessage.fromId = Utils::getLocalAddress();
    ackMessage.nextHop = message.previousNode;
    ackMessage.destinationId = message.fromId;
    ackMessage.messageType = MESSAGE_TYPE_ACK;
    ackMessage.messageId = message.messageId;
    ackMessage.previousNode = message.fromId;
    ackMessage.ttl = TTL;

    return ackMessage;
}

Message MessageService2::createAckForMessage(const PacketHeader& header) {
    Message ackMessage {};

    ackMessage.fromId = Utils::getLocalAddress();
    ackMessage.nextHop = header.previousNode;
    ackMessage.destinationId = header.destinationId;
    ackMessage.messageType = MESSAGE_TYPE_ACK;
    ackMessage.messageId = header.messageId;
    ackMessage.previousNode = ackMessage.fromId;
    ackMessage.ttl = TTL;

    return ackMessage;
}


Message MessageService2::createHelloMessage() {
    Message helloMessage{};

    helloMessage.fromId = Utils::getLocalAddress();
    helloMessage.nextHop = BROADCAST_ADDRESS;
    helloMessage.destinationId = BROADCAST_ADDRESS;
    helloMessage.messageType = MESSAGE_TYPE_HELLO;
    helloMessage.messageId = esp_random();
    helloMessage.ttl = TTL;

    return helloMessage;
}
