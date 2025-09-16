//
// Created by never on 26.03.2025.
//

#include "MessageQueue.h"

#include <mutex>

void MessageQueue::push(const Message &msg) {
    std::lock_guard lock(mtx);
    queue.push(msg);
}

bool MessageQueue::pop(Message &msg) {
    std::lock_guard lock(mtx);

    if (queue.empty()) {
        return false;
    }

    msg = queue.top();
    queue.pop();

    if (msg.messageType == MESSAGE_TYPE_HELLO) {
        std::vector<Message> filtered;
        while (!queue.empty()) {
            Message m = queue.top();
            queue.pop();

            if (m.messageType != MESSAGE_TYPE_HELLO) {
                filtered.push_back(m);
            }
        }

        for (const auto& m : filtered) {
            queue.push(m);
        }
    }

    return true;
}

bool MessageQueue::empty() const {
    std::lock_guard lock(mtx);
    return queue.empty();
}

size_t MessageQueue::size() const {
    std::lock_guard lock(mtx);
    return queue.size();
}

void MessageQueue::clear() {
    std::lock_guard lock(mtx);
    while (!queue.empty()) {
        queue.pop();
    }

    lastPoppedWasHello = false;
}

