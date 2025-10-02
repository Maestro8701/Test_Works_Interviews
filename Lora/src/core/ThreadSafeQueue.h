//
// Created by never on 23.05.2025.
//

#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H
#include <deque>
#include <mutex>

template<typename T>
class ThreadSafeQueue {
public:
    void push(const T& item) {
        std::lock_guard guard(_mutex);
        _container.push_back(item);
    }

    bool pop(T& out) {
        std::lock_guard guard(_mutex);

        if (_container.empty()) {
            return false;
        }

        out = _container.front();
        _container.pop_front();

        return true;
    }

    bool empty() const {
        std::lock_guard guard(_mutex);

        return _container.empty();
    }

    size_t size() const {
        std::lock_guard guard(_mutex);

        return _container.size();
    }

    void clear() {
        std::lock_guard guard(_mutex);

        _container.clear();
    }
private:
    mutable std::mutex _mutex;
    std::deque<T> _container;
};

#endif //THREADSAFEQUEUE_H
