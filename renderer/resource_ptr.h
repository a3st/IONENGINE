// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/exception.h>

namespace ionengine::renderer {

enum class ResourceStateType {
    Pending,
    Ok
};

template<class ResourceType, ResourceStateType StateType>
struct ResourceStateData { };

template<class ResourceType>
struct ResourceStateData<ResourceType, ResourceStateType::Ok> {
    ResourceType resource;
};

template<class ResourceType>
struct ResourceStateData<ResourceType, ResourceStateType::Pending> { };

template<class Type>
struct ResourceState {
    std::variant<
        ResourceStateData<Type, ResourceStateType::Ok>,
        ResourceStateData<Type, ResourceStateType::Pending>
    > data;
};

template<class Type>
class ResourcePtr {
public:

    ResourcePtr() = default;

    constexpr ResourcePtr(std::nullptr_t) noexcept { }

    ResourcePtr(Type&& element) {
        _ptr = std::make_shared<ResourceState<Type>>(
            ResourceStateData<Type, ResourceStateType::Ok> { .resource = std::move(element) }
        );
    }

    ResourcePtr(ResourcePtr const& other) = default;

    ResourcePtr(ResourcePtr&&) noexcept = default;

    ResourcePtr& operator=(ResourcePtr const&) = default;

    ResourcePtr& operator=(ResourcePtr&&) noexcept = default;

    uint32_t use_count() const { return _ptr.use_count(); }

    Type* operator->() const {
        assert(_ptr->data.index() == 0 && "error while accessing an unloaded resource data");
        return &(std::get<0>(_ptr->data).resource); 
    }

    Type& operator*() const { 
        assert(_ptr->data.index() == 0 && "error while accessing an unloaded resource data");
        return std::get<0>(_ptr->data).resource;
    }

    bool is_pending() const {
        return _ptr->data.index() == 2;
    }

    bool is_ok() const {
        return _ptr->data.index() == 0;
    }

    void wait() {
        while(is_pending()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void commit_ok(Type&& element) {
        _ptr->data = ResourceStateData<Type, ResourceStateType::Ok> { .resource = std::move(element) };
    }

    Type commit_pending() {
        Type element = std::move(std::get<0>(_ptr->data).resource);
        _ptr->data = ResourceStateData<Type, ResourceStateType::Pending> { };
        return std::move(element);
    }

    explicit operator bool() const noexcept { return _ptr.operator bool(); }

private:

    std::shared_ptr<ResourceState<Type>> _ptr;
};

}
