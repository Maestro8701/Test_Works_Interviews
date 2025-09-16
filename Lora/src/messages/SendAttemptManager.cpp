//
// Created by never on 28.05.2025.
//

#include "SendAttemptManager.h"

void SendAttemptManager::registerMessage(uint32_t messageId) {
    std::lock_guard lock(_mutex);
    _attempts[messageId] = SendState{};
}

void SendAttemptManager::incrementAttempt(uint32_t messageId) {
    std::lock_guard lock(_mutex);
    auto& state = _attempts[messageId];
    state.attemptCount++;

    if (state.attemptCount >= BROADCAST_THRESHOLD) {
        state.useBroadcast = true;
    } else if (state.attemptCount >= ALTERNATIVE_THRESHOLD) {
        state.fallbackToAlternative = true;
    }
}

void SendAttemptManager::markDelivered(uint32_t messageId) {
    std::lock_guard lock(_mutex);
    _attempts.erase(messageId);
}

bool SendAttemptManager::shouldFallbackToAlternative(uint32_t messageId) const {
    std::lock_guard lock(_mutex);
    const auto it = _attempts.find(messageId);
    return it != _attempts.end() && it->second.fallbackToAlternative;
}

bool SendAttemptManager::shouldUseBroadcast(uint32_t messageId) const {
    std::lock_guard lock(_mutex);
    const auto it = _attempts.find(messageId);
    return it != _attempts.end() && it->second.useBroadcast;
}

void SendAttemptManager::setLastTriedNextHop(uint32_t messageId, uint16_t nextHop) {
    std::lock_guard lock(_mutex);
    const auto it = _attempts.find(messageId);
    if (it != _attempts.end()) {
        it->second.lastTriedNextHop = nextHop;
    }
}

uint16_t SendAttemptManager::getLastTriedNextHop(uint32_t messageId) const {
    std::lock_guard lock(_mutex);
    const auto it = _attempts.find(messageId);
    return it != _attempts.end() ? it->second.lastTriedNextHop : 0;
}

uint8_t SendAttemptManager::getAttemptCount(uint32_t messageId) const {
    std::lock_guard lock(_mutex);
    const auto it = _attempts.find(messageId);
    return it != _attempts.end() ? it->second.attemptCount : 0;
}
