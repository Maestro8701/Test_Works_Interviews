//
// Created by never on 24.01.2025.
//

#include "MessageUtils.h"

#include "Utils.h"

Message MessageUtils::stringToMessage(const String* message) {
    Message parsedMessage;

    char delimiter = '|';

    int startIndex = 0;
    int delimiterIndex = message->indexOf(delimiter);

    // fromId
    parsedMessage.fromId = message->substring(startIndex, delimiterIndex).toInt();

    // destinationId
    startIndex = delimiterIndex + 1;
    delimiterIndex = message->indexOf(delimiter, startIndex);
    parsedMessage.destinationId = message->substring(startIndex, delimiterIndex).toInt();

    // messageType
    startIndex = delimiterIndex + 1;
    delimiterIndex = message->indexOf(delimiter, startIndex);
    parsedMessage.messageType = static_cast<MessageType>(message->substring(startIndex, delimiterIndex).toInt());

    // nextHop
    startIndex = delimiterIndex + 1;
    delimiterIndex = message->indexOf(delimiter, startIndex);
    parsedMessage.nextHop = message->substring(startIndex, delimiterIndex).toInt();

    // ttl
    startIndex = delimiterIndex + 1;
    delimiterIndex = message->indexOf(delimiter, startIndex);
    parsedMessage.ttl = strtoul(message->substring(startIndex, delimiterIndex).c_str(), nullptr, 10);

    // messageId
    startIndex = delimiterIndex + 1;
    delimiterIndex = message->indexOf(delimiter, startIndex);
    parsedMessage.messageId = strtoul(message->substring(startIndex, delimiterIndex).c_str(), nullptr, 10);

    // text
    startIndex = delimiterIndex + 1;
    //parsedMessage.text = message->substring(startIndex);

    return parsedMessage;
}

String MessageUtils::messageToLogString(const Message& message) {
    char buffer[256]; // Буфер достаточного размера для всех полей
    snprintf(buffer, sizeof(buffer),
        "[From:%s][To:%s][Type:%s][NextHop:%s][TTL:%u][ID:%u][Prev:%s][Time:%lu][Text:%.64s]",
        Utils::intToHexString(message.fromId),
        Utils::intToHexString(message.destinationId),
        message.typeToString(),
        Utils::intToHexString(message.nextHop),
        message.ttl,
        message.messageId,
        Utils::intToHexString(message.previousNode),
        message.timestamp,
        message.text.c_str()
    );
    return {buffer};
}

String MessageUtils::messageToString(const Message* message) {
    String result;
    result += message->fromId;
    result += "|";
    result += message->destinationId;
    result += "|";
    result += message->messageType;
    result += "|";
    result += message->nextHop;
    result += "|";
    result += message->ttl;
    result += "|";
    result += message->messageId;
    result += "|";
    //result += message->text;

    return result;
}

JsonDocument MessageUtils::serializeToJson(const Message* message) {
    JsonDocument doc;
    doc["f"] = message->fromId;
    doc["d"] = message->destinationId;
    doc["tp"] = message->messageType;
    doc["nh"] = message->nextHop;
    doc["ttl"] = message->ttl;
    doc["id"] = message->messageId;
    doc["t"] = message->text;

    return doc;
}

Message MessageUtils::deserializeFromJson(const JsonDocument& doc) {
    Message message;
    message.fromId = doc["f"].as<int>();
    message.destinationId = doc["d"].as<int>();
    message.messageType = static_cast<MessageType>(doc["tp"].as<uint8_t>());
    message.nextHop = doc["nh"].as<int>();
    message.ttl = doc["ttl"].as<int>();
    message.messageId = doc["id"].as<int>();
    //message.text = doc["t"].as<String>();

    return message;
}

bool MessageUtils::isFullEmpty(const Message& message) {
    return message.fromId == 0 && 
    message.destinationId == 0 && 
    message.messageType == MESSAGE_TYPE_HELLO && 
    message.nextHop == 0 &&
    message.messageId == 0;
}
