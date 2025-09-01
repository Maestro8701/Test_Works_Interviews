//
// Created by never on 25.01.2025.
//

#ifndef UTILS_H
#define UTILS_H
#include <Arduino.h>
#include <list>

class Utils {
public: 
    static String getLocalAddressString();

    static uint16_t getLocalAddress();

    static uint16_t getLocalAddressFromString(const String& localAddress);

    static String intToHexString(uint16_t value);

    static std::list<String> splitString(String& input, char delimiter);

    static uint16_t getLocalAddressFromString(const std::string& localAddress);
};



#endif //UTILS_H
