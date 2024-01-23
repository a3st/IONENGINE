// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine {

namespace renderer {

namespace rhi {

class FutureImpl {
public:

    virtual auto get_result() const -> bool = 0;

    virtual auto wait() -> void = 0; 
};

template<typename Type>
class Future {

    template<typename Derived>
    friend class Future;

public:

    Future() : ptr(nullptr), impl(nullptr) { }

    Future(core::ref_ptr<Type> ptr, std::unique_ptr<FutureImpl>&& impl) : ptr(ptr), impl(std::move(impl)) { }

    Future(Future&& other) : ptr(std::move(other.ptr)), impl(std::move(other.impl)) { }

    template<typename Derived>
    Future(Future<Derived> other) : ptr(other.ptr), impl(std::move(other.impl)) { }

    Future(Future const&) = delete;

    auto operator=(Future&& other) -> Future& {

        ptr = std::move(other.ptr);
        impl = std::move(other.impl);
        return *this;
    }

    template<typename Derived>
    auto operator=(Future<Derived> other) -> Future& {
        ptr = other.ptr;
        impl = std::move(other.impl);
        return *this;
    }

    auto operator=(Future const&) -> Future& = delete;

    auto get_result() const -> bool {

        return impl->get_result();
    }

    auto wait() -> void {

        impl->wait();
    }

    auto get() -> core::ref_ptr<Type> {

        if(!get_result()) {
            wait();
        }
        return ptr; 
    }

private:

    core::ref_ptr<Type> ptr;
    std::unique_ptr<FutureImpl> impl;
};

}

}

}