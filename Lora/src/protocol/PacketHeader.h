//
// Created by never on 16.05.2025.
//

#ifndef PACKETHEADER_H
#define PACKETHEADER_H

#include <cstdint>
#include <cstddef>
#include "config.h"

#pragma pack(push, 1)
struct PacketHeader {
    uint16_t fromId;
    uint16_t destinationId;
    uint16_t nextHop;
    uint8_t messageType;
    uint8_t ttl;
    uint16_t previousNode;
    uint32_t messageId;
};
#pragma pack(pop)

static_assert(sizeof(PacketHeader) == 14, "PacketHeader must be 14 bytes");
static constexpr size_t MAX_PACKET_SIZE = sizeof(PacketHeader) + MAX_MESSAGE_TEXT_SIZE;

#endif //PACKETHEADER_H
