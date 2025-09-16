//
// Created by never on 24.01.2025.
//

#ifndef MESSAGEUTILS_H
#define MESSAGEUTILS_H

#include <Arduino.h>
#include "Message.h"
#include "vector"
#include "ArduinoJson.h"

class MessageUtils
{
private:
    /* data */
public:
    static String messageToString(const Message* message);
    static Message stringToMessage(const String* text);
    static String messageToLogString(const Message& message);

    static JsonDocument serializeToJson(const Message* message);
    static Message deserializeFromJson(const JsonDocument& doc);

    static bool isFullEmpty(const Message& message);
};

#endif //MESSAGEUTILS_H
