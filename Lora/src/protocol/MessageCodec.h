//
// Created by never on 17.05.2025.
//

#ifndef MESSAGECODEC_H
#define MESSAGECODEC_H
#include "Frame.h"
#include "messages/Message.h"
#include "modules/SX127x/SX1276.h"


class MessageCodec {
public:
    static size_t encode(const Message &msg, uint8_t *outBuf, size_t outBufSize);
    static size_t encodeFrame(const Frame& frame, uint8_t* outBuf, size_t outBufSize);
    static bool decode(const uint8_t *inBuf, size_t inLen, Message &msgOut);

    static bool peekFromId(SX1276 &radio, uint16_t &outFromId);
};

#endif //MESSAGECODEC_H
