//
// Created by never on 17.05.2025.
//

#include "MessageCodec.h"
#include <cstring>
#include "PacketHeader.h"
#include "Frame.h"

size_t MessageCodec::encode(const Message& msg, uint8_t *outBuf, size_t outBufSize) {
    const size_t textLen = msg.text.size();
    const size_t total = sizeof(PacketHeader) + textLen;

    if (total > outBufSize) {
        return 0;
    }

    PacketHeader header;
    header.fromId = msg.fromId;
    header.destinationId = msg.destinationId;
    header.nextHop = msg.nextHop;
    header.previousNode = msg.previousNode;
    header.messageId = msg.messageId;
    header.ttl = msg.ttl;
    header.messageType = static_cast<uint8_t>(msg.messageType);

    std::memcpy(outBuf, &header, sizeof(header));

    if (textLen) {
        std::memcpy(outBuf + sizeof(header), msg.text.data(), textLen);
    }

    return total;
}

size_t MessageCodec::encodeFrame(const Frame& frame, uint8_t *outBuf, size_t outBufSize) {
    constexpr size_t headerSize = sizeof(PacketHeader);
    size_t totalSize = headerSize + frame.length;

    if (totalSize > outBufSize) {
        return 0;
    }

    std::memcpy(outBuf, &frame.header, headerSize);

    if (frame.length) {
        std::memcpy(outBuf + headerSize, frame.payload, frame.length);
    }

    return totalSize;
}

bool MessageCodec::decode(const uint8_t *inBuf, size_t inLen, Message &msgOut) {
    if (inLen < sizeof(PacketHeader)) {
        return false;
    }

    PacketHeader header;
    std::memcpy(&header, inBuf, sizeof(header));

    const size_t textLen = inLen - sizeof(header);

    if (inLen != sizeof(header) + textLen) {
        return false;
    }

    msgOut.fromId = header.fromId;
    msgOut.destinationId = header.destinationId;
    msgOut.nextHop = header.nextHop;
    msgOut.previousNode = header.previousNode;
    msgOut.messageId = header.messageId;
    msgOut.ttl = header.ttl;
    msgOut.messageType = static_cast<MessageType>(header.messageType);

    msgOut.text.assign(reinterpret_cast<const char*>(inBuf + sizeof(header)), textLen);

    return true;
}

bool MessageCodec::peekFromId(SX1276 &radio, uint16_t &outFromId) {
    uint8_t raw[sizeof(uint16_t)];

    if (radio.readData(raw, sizeof(raw)) != RADIOLIB_ERR_NONE) {
        return false;
    }

    outFromId = static_cast<uint16_t>(raw[0]) | (static_cast<uint16_t>(raw[1]) << 8);
    return true;
}
