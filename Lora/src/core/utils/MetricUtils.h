//
// Created by never on 06.05.2025.
//

#ifndef METRICUTILS_H
#define METRICUTILS_H
#include <Arduino.h>

#include "routing/neighborInfo.h"
#include "routing/route.h"

class MetricUtils {
public:
    static float normalizeSnr(const float snr) {
        constexpr float SNR_MIN = -20.0f;
        constexpr float SNR_MAX = 10.0f;

        const float clamped = constrain(snr, SNR_MIN, SNR_MAX);

        return (clamped - SNR_MIN) / (SNR_MAX - SNR_MIN);
    }

    static float normalizeRssi(const float rssi) {
        constexpr float RSSI_MIN = -120.0f;
        constexpr float RSSI_MAX = -40.0f;

        const float clamped = constrain(rssi, RSSI_MIN, RSSI_MAX);

        return (clamped - RSSI_MIN) / (RSSI_MAX - RSSI_MIN);
    }

    static float calculateRouteScore(const NeighborInfo& neighbor, const Route& route, const float pdr) {
        const float snrNorm = normalizeSnr(neighbor.snrHistory.get());
        const float rssiNorm = normalizeRssi(neighbor.rssiHistory.get());

        constexpr float SNR_WEIGHT = 0.7f;
        constexpr float RSSI_WEIGHT = 0.3f;
        constexpr float PENALTY_WEIGHT = 1.0f;

        const float penalty = route.penalty;
        const float score = (SNR_WEIGHT * snrNorm + RSSI_WEIGHT * rssiNorm) * pdr / (penalty  + PENALTY_WEIGHT);

        return score;
    }
};

#endif //METRICUTILS_H
