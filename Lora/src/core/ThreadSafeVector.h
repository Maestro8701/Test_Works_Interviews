//
// Created by never on 23.04.2025.
//

#ifndef THREADSAFEVECTOR_H
#define THREADSAFEVECTOR_H
#include <mutex>
#include <vector>

template<typename T>
class ThreadSafeVector {
private:
    std::vector<T> data;
    mutable std::mutex mutex;

public:
    void push_back(const T& value) {
        std::lock_guard lock(mutex);
        data.push_back(value);
    }

    void erase_if(const std::function<bool(const T&)>& predicate) {
        std::lock_guard lock(mutex);
        data.erase(std::remove_if(data.begin(), data.end(), predicate), data.end());
    }

    void remove(const T& value) {
        std::lock_guard lock(mutex);
        data.erase(std::remove(data.begin(), data.end(), value), data.end());
    }

    size_t size() const {
        std::lock_guard lock(mutex);
        return data.size();
    }

    void clear() {
        std::lock_guard lock(mutex);
        data.clear();
    }

    // Возвращает копию данных — идеально для чтения, поиска, STL-алгоритмов
    std::vector<T> snapshot() const {
        std::lock_guard lock(mutex);
        return data;
    }

    // Итераторы безопасны, потому что работают с snapshot'ом (копией)
    auto begin() const {
        return snapshot().begin();
    }

    auto end() const {
        return snapshot().end();
    }

    // Можно безопасно обойти через индекс
    T operator[](size_t index) const {
        std::lock_guard lock(mutex);
        if (index >= data.size()) {
            throw std::out_of_range("ThreadSafeVector::operator[] - index out of range");
        }
        return data.at(index);
    }

    // Доступ с передачей функции под мьютексом
    void locked(const std::function<void(std::vector<T>&)>& func) {
        std::lock_guard lock(mutex);
        func(data);
    }
};


#endif //THREADSAFEVECTOR_H
