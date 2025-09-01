//
// Created by never on 08.06.2025.
//

#include "RoutingService.h"
#include "core/utils/MetricUtils.h"

RoutingService::RoutingService(SendAttemptManager& sendAttemptManager)
    : _sendAttemptManager(sendAttemptManager) {}

void RoutingService::analyzePacket(uint16_t nodeId, float rssi, float snr) {
    _routingTable.createOrUpdate(nodeId, rssi, snr);
}

void RoutingService::markSuccess(uint16_t from, uint16_t to, uint16_t nextHop) {
    Route* route = findOrCreateRoute(Route{from, to, nextHop}.getRouteId(), from, to, nextHop);
    if (route) {
        route->reportSuccess();
    }
}

void RoutingService::markFailure(uint16_t from, uint16_t to, uint16_t nextHop) {
    Route* route = findOrCreateRoute(Route{from, to, nextHop}.getRouteId(), from, to, nextHop);
    if (route) {
        route->reportFailure();
    }
}

Route* RoutingService::findOrCreateRoute(uint32_t routeId, uint16_t fromNode, uint16_t toNode, uint16_t nextHop) {
    auto it = _routes.find(routeId);
    if (it != _routes.end()) {
        return &it->second;
    }

    Route newRoute;
    newRoute.fromNode = fromNode;
    newRoute.toNode = toNode;
    newRoute.nextHop = nextHop;

    _routes[newRoute.getRouteId()] = std::move(newRoute);
    return &_routes[newRoute.getRouteId()];
}

size_t RoutingService::removeExpiredNeighbors() {
    return _routingTable.removeExpired(HEARTBEAT_INTERVAL);
}

uint16_t RoutingService::getBestNextHop(const Message& message) {
    const uint32_t messageId = message.messageId;
    const uint16_t destination = message.destinationId;
    const uint16_t previousNode = message.previousNode;

    // 1. Прямой сосед?
    if (_routingTable.hasNeighbor(destination)) {
        return destination;
    }

    // 2. Broadcast — крайняя мера
    if (_sendAttemptManager.shouldUseBroadcast(messageId)) {
        return BROADCAST_ADDRESS;
    }

    // 3. Исключаем предыдущий nextHop при fallback
    uint16_t excludedHop = 0;
    if (_sendAttemptManager.shouldFallbackToAlternative(messageId)) {
        excludedHop = _sendAttemptManager.getLastTriedNextHop(messageId);
    }

    float bestScore = -1.0f;
    uint16_t bestHop = BROADCAST_ADDRESS;

    for (const auto& [routeId, route] : _routes) {
        if (route.toNode != destination) continue;
        if (route.nextHop == excludedHop) continue;
        if (route.nextHop == previousNode) continue;  // защита от циклов
        if (route.nextHop == BROADCAST_ADDRESS) continue;
        if (!_routingTable.hasNeighbor(route.nextHop)) continue;

        const NeighborInfo neighbor = _routingTable.getNeighborById(route.nextHop);
        const float pdr = 1.0f;  // заменить при наличии статистики
        const float score = MetricUtils::calculateRouteScore(neighbor, route, pdr);

        if (score > bestScore) {
            bestScore = score;
            bestHop = route.nextHop;
        }
    }

    if (bestHop != BROADCAST_ADDRESS) {
        _sendAttemptManager.setLastTriedNextHop(messageId, bestHop);
    }

    return bestHop;
}


uint16_t RoutingService::getBestNextHop(const PacketHeader& header) {
    const uint16_t destination = header.destinationId;
    const uint16_t previousNode = header.previousNode;

    // 1. Прямой сосед
    if (_routingTable.hasNeighbor(destination)) {
        return destination;
    }

    float bestScore = -1.0f;
    uint16_t bestHop = BROADCAST_ADDRESS;

    for (const auto& [id, route] : _routes) {
        if (route.toNode != destination) continue;
        if (route.nextHop == previousNode) continue;
        if (route.nextHop == BROADCAST_ADDRESS) continue;
        if (!_routingTable.hasNeighbor(route.nextHop)) continue;

        const NeighborInfo neighbor = _routingTable.getNeighborById(route.nextHop);
        const float pdr = 1.0f;
        const float score = MetricUtils::calculateRouteScore(neighbor, route, pdr);

        if (score > bestScore) {
            bestScore = score;
            bestHop = route.nextHop;
        }
    }

    return bestHop;
}

