// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

template<class Type>
struct Handle {

    uint32_t id{std::numeric_limits<uint32_t>::max()};

    Handle() = default;

    Handle(uint32_t const _id) : id(_id) { }

    Handle(Handle const& other) : id(other.id) { }
    
    Handle(Handle&& other) noexcept : id(other.id) { }

    Handle& operator=(Handle const& other) {

        id = other.id;
        return *this;
    }

    Handle& operator=(Handle&& other) noexcept {

        id = other.id;
        return *this;
    }

    bool operator==(Handle const& other) const { return id == other.id; }

    bool operator!=(Handle const& other) const { return id != other.id; }
};

}

#ifndef INVALID_HANDLE
#define INVALID_HANDLE(Type) ionengine::Handle<Type>()
#endif // INVALID_HANDLE
