//
// Created by never on 19.05.2025.
//

#ifndef ROUTINGTABLE2_H
#define ROUTINGTABLE2_H
#include "NeighborInfo.h"
#include "Route.h"


class RoutingTable2 {
public:
    void createOrUpdate(uint16_t nodeId, float rssi, float snr);
    void addNeighbor(NeighborInfo neighbor);
    void removeNeighbor(uint16_t neighborId);
    NeighborInfo getNeighborById(uint16_t neighborId) const;
    bool hasNeighbor(uint16_t neighborId) const;
    size_t removeExpired(unsigned long neighborTimeout);
    size_t getNeighborsCount();
private:
    std::vector<NeighborInfo> _neighbors;
};



#endif //ROUTINGTABLE2_H
