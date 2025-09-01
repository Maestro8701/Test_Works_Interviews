//
// Created by never on 19.05.2025.
//

#include "RoutingTable2.h"

#include "config.h"

void RoutingTable2::createOrUpdate(const uint16_t nodeId, const float rssi, const float snr) {
    for (auto& item : _neighbors) {
        if (item.nodeId == nodeId) {
            item.rssiHistory.update(rssi);
            item.snrHistory.update(snr);
            item.lastInteraction = millis();
            item.missedHelloCount = 0;

            return;
        }
    }

    NeighborInfo newNeighbor;
    newNeighbor.nodeId = nodeId;
    newNeighbor.rssiHistory.update(rssi);
    newNeighbor.snrHistory.update(snr);
    newNeighbor.lastInteraction = millis();
    newNeighbor.missedHelloCount = 0;

    _neighbors.push_back(newNeighbor);
}

void RoutingTable2::addNeighbor(NeighborInfo neighbor) {
    _neighbors.push_back(neighbor);
}

void RoutingTable2::removeNeighbor(uint16_t neighborId) {
    _neighbors.erase(
        std::remove_if(
            _neighbors.begin(),
            _neighbors.end(),
            [neighborId](const NeighborInfo& n) {
        return n.nodeId == neighborId;
    }), _neighbors.end());
}

/**
 * @brief Возвращает соседа по его id
 * @param neighborId id соседа
 * @note Настоятельно рекомендуется использовать <b>hasNeighbor(const uint16_t neighborId)</b> до вызова этого метода!
 **/
NeighborInfo RoutingTable2::getNeighborById(const uint16_t neighborId) const {
    for (const auto& neighbor : _neighbors) {
        if (neighbor.nodeId == neighborId) {
            return neighbor;
        }
    }

    return NeighborInfo();
}

bool RoutingTable2::hasNeighbor(const uint16_t neighborId) const {
    for (auto& neighbor : _neighbors) {
        if (neighbor.nodeId == neighborId) {
            return true;
        }
    }
    return false;
}

size_t RoutingTable2::removeExpired(unsigned long neighborTimeout = HEARTBEAT_INTERVAL) {
    const unsigned long now = millis();

    size_t removed = 0;

    for (auto it = _neighbors.begin(); it != _neighbors.end(); ) {
        if (now - it->lastInteraction > neighborTimeout) {
            it = _neighbors.erase(it);
            removed++;
        } else {
            ++it;
        }
    }

    return removed;
}

size_t RoutingTable2::getNeighborsCount() {
    return _neighbors.size();
}
