// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/exception.h>

namespace ionengine::renderer {

template<class Type>
struct CacheState {
    Type resource;
    bool is_pending;
};

template<class Type>
class CachePtr {
public:

    CachePtr() = default;

    constexpr CachePtr(std::nullptr_t) noexcept { }

    CachePtr(Type resource) {
        _ptr = std::make_shared<CacheState<Type>>(std::move(resource), true);
    }

    CachePtr(CachePtr const& other) = default;

    CachePtr(CachePtr&&) noexcept = default;

    CachePtr& operator=(CachePtr const&) = default;

    CachePtr& operator=(CachePtr&&) noexcept = default;

    uint32_t use_count() const { return _ptr.use_count(); }

    Type* operator->() const {

        if(is_pending()) {
            throw lib::Exception(std::format("Error while accessing an unloaded resource"));
        }
        return &_ptr->resource; 
    }

    Type& operator*() const { 

        if(is_pending()) {
            throw lib::Exception(std::format("Error while accessing an unloaded resource"));
        }
        return _ptr->resource;
    }

    bool is_pending() const {
        return _ptr->is_pending;
    }

    bool is_ok() const {
        return !_ptr->is_pending;
    }

    void commit_ok() {
        _ptr->is_pending = false;
    }

    explicit operator bool() const noexcept { return _ptr.operator bool(); }

private:

    std::shared_ptr<CacheState<Type>> _ptr;
};

}
