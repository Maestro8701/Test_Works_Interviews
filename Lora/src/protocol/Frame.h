//
// Created by never on 21.05.2025.
//

#ifndef FRAME_H
#define FRAME_H
#include "PacketHeader.h"
#include "cstddef"

struct Frame {
    PacketHeader header;
    size_t length;
    uint8_t payload[MAX_MESSAGE_TEXT_SIZE];
    float rssi;
    float snr;
};

#endif //FRAME_H
