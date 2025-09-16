//
// Created by never on 20.05.2025.
//

#ifndef EVENTLISTENER_H
#define EVENTLISTENER_H
#include "Event.h"

class EventListener {
public:
    virtual ~EventListener() = default;

    virtual void onEvent(const Event &event) = 0;
};

#endif //EVENTLISTENER_H
