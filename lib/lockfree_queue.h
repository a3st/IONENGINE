// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

template<class Type, size_t Size>
class ConcurrentQueue {
public:

    ConcurrentQueue() = default;

    bool try_push(Type const& element) {
        
        const size_t current = _tail.load();
        const size_t next = (current + 1) % Capacity;
        if(next != _head.load()) {
            _array[current] = element;
            _tail.store(next);
            return true;
        }
        return false;
    }

    bool try_pop(Type& element) {

        const size_t current = _head.load();
        if(current == _tail.load()) {
            return false;
        }
        element = _array[current];
        _head.store((current + 1) % Capacity);
        return true;
    }

    bool empty() {

        const size_t current = _head.load();
        return current == _tail.load();
    }

private:

    enum { Capacity = Size + 1 };

    std::atomic<size_t> _tail{0};
    std::atomic<size_t> _head{0};
    std::array<Type, Capacity> _array;
};

}