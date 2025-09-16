//
// Created by never on 24.01.2025.
//

#ifndef MESSAGE_H
#define MESSAGE_H

#include <Arduino.h>

enum MessageType : uint8_t {
    MESSAGE_TYPE_HELLO = 0,
    MESSAGE_TYPE_TEXT = 1,
    MESSAGE_TYPE_BYE = 2,
    MESSAGE_TYPE_ERROR = 3,
    MESSAGE_TYPE_UNKNOWN = 4,
    MESSAGE_TYPE_ACK = 5
};

struct Message {
    uint16_t fromId;
    uint16_t destinationId;
    uint16_t nextHop;
    MessageType messageType;
    uint8_t ttl;
    uint32_t messageId;
    uint16_t previousNode;
    unsigned long timestamp;
    std::string text;

    [[nodiscard]] String typeToString() const {
        switch (messageType) {
            case MESSAGE_TYPE_HELLO:
                return "HELLO";
            case MESSAGE_TYPE_TEXT:
                return "TEXT";
            case MESSAGE_TYPE_BYE:
                return "BYE";
            case MESSAGE_TYPE_ERROR:
                return "ERROR";
            case MESSAGE_TYPE_UNKNOWN:
                return "UNKNOWN";
            case MESSAGE_TYPE_ACK:
                return "ACK";
            default:
                return "UNKNOWN";
        }
    }

    [[nodiscard]] int getPriority() const {
        switch (messageType) {
            case MESSAGE_TYPE_HELLO:
                return 80;
            case MESSAGE_TYPE_TEXT:
                return 90;
            case MESSAGE_TYPE_BYE:
                return 2;
            case MESSAGE_TYPE_ERROR:
                return 3;
            case MESSAGE_TYPE_UNKNOWN:
                return 4;
            case MESSAGE_TYPE_ACK:
                return 100;
            default:
                return 0;
        }
    }

    bool operator<(const Message& other) const {
        return getPriority() < other.getPriority();
    }

    bool operator==(const Message& other) const {
        return messageId == other.messageId && timestamp == other.timestamp;
    }
};

// struct Message {
//     uint16_t fromId;
//     uint16_t destinationId;
//     MessageType messageType;
//     uint16_t nextHop;
//     uint8_t ttl;
//     uint32_t messageId;
//     uint16_t previousNode;
//     unsigned long timestamp;
//     String text;
//
//     [[nodiscard]] String typeToString() const {
//         switch (messageType) {
//             case MESSAGE_TYPE_HELLO:
//                 return "HELLO";
//             case MESSAGE_TYPE_TEXT:
//                 return "TEXT";
//             case MESSAGE_TYPE_BYE:
//                 return "BYE";
//             case MESSAGE_TYPE_ERROR:
//                 return "ERROR";
//             case MESSAGE_TYPE_UNKNOWN:
//                 return "UNKNOWN";
//             case MESSAGE_TYPE_ACK:
//                 return "ACK";
//             default:
//                 return "UNKNOWN";
//         }
//     }
//
//     [[nodiscard]] int getPriority() const {
//         switch (messageType) {
//             case MESSAGE_TYPE_HELLO:
//                 return 90;
//             case MESSAGE_TYPE_TEXT:
//                 return 80;
//             case MESSAGE_TYPE_BYE:
//                 return 2;
//             case MESSAGE_TYPE_ERROR:
//                 return 3;
//             case MESSAGE_TYPE_UNKNOWN:
//                 return 4;
//             case MESSAGE_TYPE_ACK:
//                 return 100;
//             default:
//                 return 0;
//         }
//     }
//
//     bool operator<(const Message& other) const {
//         return getPriority() < other.getPriority();
//     }
//
//     bool operator==(const Message& other) const {
//         return messageId == other.messageId && timestamp == other.timestamp;
//     }
// };

#endif //MESSAGE_H
