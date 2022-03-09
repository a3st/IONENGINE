// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

template<class Type> 
class HandleAllocator;

template<class Type>
class Handle {
public:

    Handle() = default;

    Handle(uint32_t const id) : _id(id) { }

    Handle(Handle const& other) : _id(other._id) { }
    
    Handle(Handle&& other) noexcept : _id(other._id) { }

    Handle& operator=(Handle const& other) {

        _id = other._id;
        return *this;
    }

    Handle& operator=(Handle&& other) noexcept {

        _id = other._id;
        return *this;
    }

    bool operator==(Handle const& other) { return _id == other._id; }

    bool operator!=(Handle const& other) { return _id != other._id; }

private:

    uint32_t _id{std::numeric_limits<uint32_t>::max()};

    friend class HandleAllocator<Type>;
};

}

#ifndef INVALID_HANDLE
#define INVALID_HANDLE(Type) ionengine::Handle<Type>()
#endif