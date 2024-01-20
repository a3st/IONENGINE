// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine {

namespace renderer {

namespace rhi {

template<typename Type>
class Future {

    template<typename Derived>
    friend class Future;

public:

    Future() : ptr(nullptr) { }

    Future(core::ref_ptr<Type> ptr) : ptr(ptr) { }

    Future(Future&& other) : ptr(std::move(other.ptr)) { }

    template<typename Derived>
    Future(Future<Derived>&& other) : ptr(other.ptr) { }

    Future(Future const&) = delete;

    auto operator=(Future&& other) -> Future& {

        ptr = std::move(other.ptr);
        return *this;
    }

    template<typename Derived>
    auto operator=(Future<Derived>&& other) -> Future& {
        ptr = other.ptr;
        return *this;
    }

    auto operator=(Future const&) -> Future& = delete;

    auto get_result() const -> bool {

        return true;
    }

private:

    core::ref_ptr<Type> ptr;
};

}

}

}