//
// Created by never on 26.03.2025.
//

#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
#include <mutex>
#include <queue>

#include "Message.h"


class MessageQueue {
public:
    void push(const Message& msg);
    bool pop(Message& msg);
    bool empty() const;
    size_t size() const;
    void clear();
private:
    std::priority_queue<Message> queue;
    bool lastPoppedWasHello = false;
    mutable std::mutex mtx;
};



#endif //MESSAGEQUEUE_H
