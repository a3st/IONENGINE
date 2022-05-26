// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/exception.h>

namespace ionengine::renderer {

enum class ResourceStateType {
    Pending,
    Common,
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

template<class ResourceType>
struct ResourceStateData<ResourceType, ResourceStateType::Common> {
    ResourceType resource;
};

template<class Type>
struct ResourceState {
    std::variant<
        ResourceStateData<Type, ResourceStateType::Ok>,
        ResourceStateData<Type, ResourceStateType::Pending>,
        ResourceStateData<Type, ResourceStateType::Common>
    > data;
};

template<class Type>
class ResourcePtr {
public:

    ResourcePtr() = default;

    constexpr ResourcePtr(std::nullptr_t) noexcept { }

    ResourcePtr(Type&& element) {
        _ptr = std::make_shared<ResourceState<Type>>(
            ResourceStateData<Type, ResourceStateType::Common> { .resource = std::move(element) }
        );
    }

    ResourcePtr(ResourcePtr const& other) = default;

    ResourcePtr(ResourcePtr&&) noexcept = default;

    ResourcePtr& operator=(ResourcePtr const&) = default;

    ResourcePtr& operator=(ResourcePtr&&) noexcept = default;

    uint32_t use_count() const { return _ptr.use_count(); }

    Type* operator->() const {
        assert((_ptr->data.index() == 0 || _ptr->data.index() == 2) && "error while accessing an unloaded resource data");

        Type* resource = nullptr;

        auto state_visitor = make_visitor(
            [&](ResourceStateData<Type, ResourceStateType::Ok>& data) {
                resource = &data.resource;
            },
            [&](ResourceStateData<Type, ResourceStateType::Common>& data) {
                resource = &data.resource;
            },
            [&](ResourceStateData<Type, ResourceStateType::Pending>& data) { }
        );

        std::visit(state_visitor, _ptr->data);
        return resource; 
    }

    Type& operator*() const { 
        assert((_ptr->data.index() == 0 || _ptr->data.index() == 2) && "error while accessing an unloaded resource data");
        
        Type* resource = nullptr;

        auto state_visitor = make_visitor(
            [&](ResourceStateData<Type, ResourceStateType::Ok>& data) {
                resource = &data.resource;
            },
            [&](ResourceStateData<Type, ResourceStateType::Common>& data) {
                resource = &data.resource;
            },
            [&](ResourceStateData<Type, ResourceStateType::Pending>& data) { }
        );

        std::visit(state_visitor, _ptr->data);
        return *resource; 
    }

    bool is_pending() const {
        return _ptr->data.index() == 1;
    }

    bool is_ok() const {
        return _ptr->data.index() == 0;
    }

    bool is_common() const {
        return _ptr->data.index() == 2;
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
        Type element = std::move(std::get<2>(_ptr->data).resource);
        _ptr->data = ResourceStateData<Type, ResourceStateType::Pending> { };
        return std::move(element);
    }

    explicit operator bool() const noexcept { return _ptr.operator bool(); }

private:

    std::shared_ptr<ResourceState<Type>> _ptr;
};

}
