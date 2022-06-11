// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

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

    std::shared_mutex mutex;

    std::atomic<bool> is_wait_for_upload;

    Type const& get() const {
        return std::get<ResourceStateData<Type, ResourceStateType::Ok>>(data).resource;
    }

    Type& get() {
        return std::get<ResourceStateData<Type, ResourceStateType::Ok>>(data).resource;
    }

    bool is_ok() const {
        std::shared_lock lock(mutex);
        return data.index() == 0;
    }

    bool is_pending() const {
        std::shared_lock lock(mutex);
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
