//
// Created by never on 08.06.2025.
//

#ifndef ROUTING_SERVICE_H
#define ROUTING_SERVICE_H

#include "Route.h"
#include "RoutingTable2.h"
#include <unordered_map>

#include "messages/Message.h"
#include "messages/SendAttemptManager.h"
#include "protocol/PacketHeader.h"

class RoutingService {
public:
    explicit RoutingService(SendAttemptManager& sendAttemptManager);

    uint16_t getBestNextHop(const Message& message);
    uint16_t getBestNextHop(const PacketHeader& header);

    void analyzePacket(uint16_t nodeId, float rssi, float snr);

    void markSuccess(uint16_t from, uint16_t to, uint16_t nextHop);
    void markFailure(uint16_t from, uint16_t to, uint16_t nextHop);

    Route* findOrCreateRoute(uint32_t routeId, uint16_t fromNode, uint16_t toNode, uint16_t nextHop);

    size_t removeExpiredNeighbors();

private:
    RoutingTable2 _routingTable;
    std::unordered_map<uint32_t, Route> _routes;
    SendAttemptManager& _sendAttemptManager;
};

#endif // ROUTING_SERVICE_H
