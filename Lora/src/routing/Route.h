//
// Created by never on 24.04.2025.
//

#ifndef ROUTE_H
#define ROUTE_H

#include <cstdint>

#include "../core/utils/MathUtils.h"

struct Route {
    uint16_t fromNode{};
    uint16_t toNode{};
    uint16_t nextHop{};

    float penalty = 0.0f;

    [[nodiscard]] bool isDirect() const {
        return toNode == nextHop;
    }

    uint32_t getRouteId() const {
        return static_cast<uint32_t>(fromNode) << 20 ^
               static_cast<uint32_t>(toNode) << 10 ^
               static_cast<uint32_t>(nextHop);
    }

    void reportSuccess() {
        penalty = MathUtils::exponentialSmoothing(penalty, 1.0f, 0.3f);
    }

    void reportFailure() {
        penalty = MathUtils::exponentialSmoothing(penalty, 0.0f, 0.4f);
    }

    bool operator==(const Route &other) const {
        return
            fromNode == other.fromNode &&
            toNode == other.toNode &&
            nextHop == other.nextHop;
    }
};

// template<>
// struct std::hash<Route> {
//     std::size_t operator()(const Route &r) const noexcept {
//         return (r.fromNode << 1) ^
//                (r.toNode << 2) ^
//                (r.nextHop << 3);
//     }
// };

#endif //ROUTE_H
