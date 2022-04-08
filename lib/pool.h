// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

class Handle {
public:

    Handle() = default;

    Handle(uint32_t const index, uint32_t const generation) : _index(index), _generation(generation) { }

    Handle(Handle const& other) : _index(other._index), _generation(generation) { }
    
    Handle(Handle&& other) noexcept : _index(other._index), _generation(generation) { }

    Handle& operator=(Handle const& other) {

        _index = other._index;
        _generation = other._generation;
        return *this;
    }

    Handle& operator=(Handle&& other) noexcept {

        _index = other._index;
        _generation = other._generation;
        return *this;
    }

    bool operator==(Handle const& other) const { return _index == other._index; }

    bool operator!=(Handle const& other) const { return _index != other._index; }

    uint32_t index() const { return _index; }
    
    uint32_t generation() const { return _generation; }

private:

    uint32_t _index{std::numeric_limits<uint32_t>::max()};
    uint32_t _generation{0};
};

#ifndef INVALID_HANDLE
#define INVALID_HANDLE(Type) ionengine::Handle<Type>()
#endif // INVALID_HANDLE

template<class Type, class Container = std::vector<Type>>
struct Pool {




};

}
