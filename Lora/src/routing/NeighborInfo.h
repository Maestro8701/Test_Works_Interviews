//
// Created by never on 18.02.2025.
//

#ifndef NEIGHBORINFO_H
#define NEIGHBORINFO_H
#include  <Arduino.h>

#include "MetricHistory.h"

struct NeighborInfo {
    uint16_t nodeId{};
    MetricHistory rssiHistory;
    MetricHistory snrHistory;
    uint16_t missedHelloCount = 0;
    unsigned long lastInteraction{};
};

#endif //NEIGHBORINFO_H
