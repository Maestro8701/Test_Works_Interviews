//
// Created by never on 21.05.2025.
//

#include "PacketProcessor.h"

#include <cstring>

#include "DebugLog.h"
#include "Frame.h"
#include "MessageCodec.h"
#include "messages/Message.h"

void PacketProcessor::onEvent(const Event& event) {
    if (event.type != EventType::RAW_PACKET) {
        return;
    }

    LOG_INFO("RAW PACKET СОБЫТИЕ ПОЛУЧЕНО");

    if (!event.data) {
        LOG_INFO("EVENT DATA IS NULL");
        return;
    }

    auto frame = event.get<Frame>();

    if (frame == nullptr) {
        return;
    }

    size_t packetLen = sizeof(PacketHeader) + frame->length;
    auto* buffer = static_cast<uint8_t*>(pvPortMalloc(packetLen));

    if (buffer == nullptr) {
        return;
    }

    memcpy(buffer, &frame->header, sizeof(PacketHeader));

    if (frame->length) {
        memcpy(buffer + sizeof(PacketHeader), frame->payload, frame->length);
    }

    // Message* msg = new Message();
    auto msg = std::make_shared<Message>();

    bool isDecoded = MessageCodec::decode(buffer, packetLen, *msg);

    vPortFree(buffer);

    if (!isDecoded) {
        return;
    }

    msg->timestamp = millis();

    LOG_INFO(msg->messageId);

    EventType publishType = (msg->messageType == MESSAGE_TYPE_ACK)
        ? EventType::ACK_RECEIVED
        : EventType::MESSAGE_READY;

    Event messageReadyEvent = Event::make(publishType, msg);

    _eventBus.emit(messageReadyEvent);
}
