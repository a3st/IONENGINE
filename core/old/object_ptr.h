// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

template<class Type>
struct ObjectData {
    Type object;
};

template<class Type>
class ObjectPtr {
public:

    ObjectPtr() = default;

    constexpr ObjectPtr(std::nullptr_t) noexcept { }

    ObjectPtr(Type&& element) {
        _ptr = std::make_shared<ObjectData<Type>>(std::move(element));
    }

    ObjectPtr(ObjectPtr const&) = default;

    ObjectPtr(ObjectPtr&&) noexcept = default;

    ObjectPtr& operator=(ObjectPtr const&) = default;

    ObjectPtr& operator=(ObjectPtr&&) noexcept = default;

    uint32_t use_count() const { return _ptr.use_count(); }

    Type* operator->() const {
        return &_ptr->object; 
    }

    Type& operator*() const { 
        return _ptr->object; 
    }

    explicit operator bool() const noexcept { return _ptr.operator bool(); }

private:

    std::shared_ptr<ObjectData<Type>> _ptr;
};

}
