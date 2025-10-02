//
// Created by never on 25.01.2025.
//

#include <Arduino.h>
#include "WiFi.h"
#include "../../Utils.h"
#include <list>

uint16_t Utils::getLocalAddress() {
    uint8_t mac[6];

    WiFi.macAddress(mac);

    uint16_t localAddress = (mac[4] << 8 | mac[5]);

    return localAddress;
}

String Utils::getLocalAddressString() {
    return String(getLocalAddress(), HEX);
}

uint16_t Utils::getLocalAddressFromString(const String& localAddress) {
    if (localAddress.length() == 0) {
        return 0;
    }

    uint16_t parsedAddress = strtoul(localAddress.c_str(), nullptr, 16);

    return parsedAddress;
}

String Utils::intToHexString(uint16_t value) {
    char hex[5];

    sprintf(hex, "%04x", value);

    return {hex};
}

std::list<String> Utils::splitString(String& input, char delimiter) {
    std::list<String> tokens;
    size_t start = 0;
    size_t len = input.length();

    for (size_t i = 0; i < len; i++)
    {
        if (input[i] == delimiter)
        {
            tokens.push_back(input.substring(start, i));
            start = i + 1;
        }
    }

    if (start < len)
    {
        tokens.push_back(input.substring(start));
    }

    return tokens;
}

uint16_t Utils::getLocalAddressFromString(const std::string &localAddress) {
    if (localAddress.empty()) {
        return 0;
    }

    const uint16_t parsedAddress = static_cast<uint16_t>(
        std::strtoul(localAddress.c_str(), nullptr, 16)
    );

    return parsedAddress;
}
