//
// Created by never on 06.05.2025.
//

#ifndef METRICHISTORY_H
#define METRICHISTORY_H
#include "../core/utils/MathUtils.h"

struct MetricHistory {
    float emaValue = 0.0f;
    float alpha = 0.4f;

    void update(const float newValue) {
        emaValue = MathUtils::exponentialSmoothing(emaValue, newValue, alpha);
    }

    [[nodiscard]] float get() const { return emaValue; }
};

#endif //METRICHISTORY_H
