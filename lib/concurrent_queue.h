// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

template<class Type, size_t Size>
class ConcurrentQueue {
public:

    ConcurrentQueue() {
        
        _data.resize(_capacity);
    }

    ConcurrentQueue(ConcurrentQueue const& other) {

        _data = other.data;
        _head.store(other._head.load());
        _tail.store(other._tail.load());
    }

    ConcurrentQueue(ConcurrentQueue&& other) noexcept {

        _data = other._data;
        _head.store(other._head.load());
        _tail.store(other._tail.load());
    }

    ConcurrentQueue& operator=(ConcurrentQueue const& other) {

        _data = other.data;
        _head.store(other._head.load());
        _tail.store(other._tail.load());
        return *this;
    }

    ConcurrentQueue& operator=(ConcurrentQueue&& other) noexcept {

        _data = other.data;
        _head.store(other._head.load());
        _tail.store(other._tail.load());
        return *this;
    }

    bool try_push(Type const& element) {
        
        size_t const current = _tail.load();
        size_t const next = (current + 1) % _capacity;

        if(next != _head.load()) {
            _data[current] = element;
            _tail.store(next);
            return true;
        }

        return false;
    }

    bool try_push(Type&& element) {
        
        size_t const current = _tail.load();
        size_t const next = (current + 1) % _capacity;

        if(next != _head.load()) {
            _data[current] = std::move(element);
            _tail.store(next);
            return true;
        }

        return false;
    }

    bool try_pop(Type& element) {

        size_t const current = _head.load();

        if(current == _tail.load()) {
            return false;
        }
        
        element = _data[current];
        _head.store((current + 1) % _capacity);
        return true;
    }

    bool empty() const {

        size_t const current = _head.load();
        return current == _tail.load();
    }

    size_t size() const {

        size_t const current = _head.load();
        return _tail.load() - current;
    }

private:

    size_t const _capacity = Size + 1;

    std::atomic<size_t> _tail{0};
    std::atomic<size_t> _head{0};
    std::vector<Type> _data;
};

}
