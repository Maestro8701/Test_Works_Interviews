//
// Created by never on 28.05.2025.
//

#ifndef SENDATTEMPTMANAGER_H
#define SENDATTEMPTMANAGER_H
#include <mutex>
#include <unordered_map>


class SendAttemptManager {
public:
    struct SendState {
        uint8_t attemptCount = 0;
        uint16_t lastTriedNextHop = 0;
        bool fallbackToAlternative = false;
        bool useBroadcast = false;
    };

    void registerMessage(uint32_t messageId);
    void incrementAttempt(uint32_t messageId);
    void markDelivered(uint32_t messageId);
    bool shouldFallbackToAlternative(uint32_t messageId) const;
    bool shouldUseBroadcast(uint32_t messageId) const;
    void setLastTriedNextHop(uint32_t messageId, uint16_t nextHop);
    uint16_t getLastTriedNextHop(uint32_t messageId) const;
    uint8_t getAttemptCount(uint32_t messageId) const;

private:
    mutable std::mutex _mutex;
    std::unordered_map<uint32_t, SendState> _attempts;

    static constexpr uint8_t ALTERNATIVE_THRESHOLD = 3;
    static constexpr uint8_t BROADCAST_THRESHOLD = 6;
};



#endif //SENDATTEMPTMANAGER_H
