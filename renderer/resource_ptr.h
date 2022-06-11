// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/proxy_mutex.h>

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

    std::mutex mutex;

    std::atomic<bool> is_wait_for_upload;

    Type const& as_const_ok() const {
        return std::get<ResourceStateData<Type, ResourceStateType::Ok>>(data).resource;
    }

    lib::ProxyMutex<Type> as_ok() {
        return lib::ProxyMutex<Type>(&std::get<ResourceStateData<Type, ResourceStateType::Ok>>(data).resource, mutex);
    }

    bool is_ok() const {
        return data.index() == 0;
    }

    bool is_pending() const {
        return data.index() == 1;
    }

    void commit_ok(Type&& element) {
        std::lock_guard lock(mutex);
        data = ResourceStateData<Type, ResourceStateType::Ok> { .resource = std::move(element) };
    }

    Type commit_pending() {
        std::lock_guard lock(mutex);
        Type element = std::move(std::get<ResourceStateData<Type, ResourceStateType::Ok>>(data).resource);
        data = ResourceStateData<Type, ResourceStateType::Pending> { };
        return std::move(element);
    }
};

template<class Type>
using ResourcePtr = std::shared_ptr<ResourceState<Type>>;

template<class Type>
inline ResourcePtr<Type> make_resource_ptr(Type&& from) {
    return ResourcePtr<Type>(new ResourceState<Type> { .data = ResourceStateData<Type, ResourceStateType::Ok> { .resource = std::move(from) } });
}

}
