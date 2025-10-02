//
// Created by never on 23.05.2025.
//

#ifndef ACKROUTEMANAGER_H
#define ACKROUTEMANAGER_H
#include <mutex>
#include <unordered_map>

class AckRouteManager {
public:
    void registerRoute(uint32_t messageId, uint16_t previousNode) {
        std::lock_guard guard(_mutex);

        _container.emplace(messageId, previousNode);
    }

    uint16_t popNodeIdForMessage(const uint32_t messageId) {
        std::lock_guard guard(_mutex);

        const uint16_t id = _container[messageId];

        _container.erase(messageId);

        return id;
    }

private:
    mutable std::mutex _mutex;
    std::unordered_map<uint32_t, uint16_t> _container;
};

#endif //ACKROUTEMANAGER_H
