//
// Created by never on 06.05.2025.
//

#ifndef MATHUTILS_H
#define MATHUTILS_H

class MathUtils {
public:
    static float exponentialSmoothing(
        const float previousSmoothedValue,
        const float newValue,
        float alpha) {
        if (alpha < 0.0f) alpha = 0.0f;
        if (alpha > 1.0f) alpha = 1.0f;

        return alpha * newValue + (1.0f - alpha) * previousSmoothedValue;
    }
};

#endif //MATHUTILS_H
