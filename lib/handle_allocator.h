// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/handle.h>

namespace ionengine {

template<class Type>
class HandleAllocator {
public:

    HandleAllocator() = default;

    HandleAllocator(HandleAllocator const&) = delete;

    HandleAllocator(HandleAllocator&&) noexcept = delete;

    HandleAllocator& operator=(HandleAllocator const&) = delete;

    HandleAllocator& operator=(HandleAllocator&&) noexcept = delete;

    Handle<Type> allocate(Type const& element) {

        const size_t cur_offset = _offset;
        _instances[static_cast<uint32_t>(cur_offset)] = element;
        ++_offset;
        return Handle<Type>(static_cast<uint32_t>(cur_offset));
    }

    void deallocate(Handle<Type> const& handle) {

        auto it = _instances.find(handle._id);
        _instances.erase(it);
    }

    void reset() {
        
        _instances.clear();
    }

    Type& get(Handle<Type> const& handle) {
        
        auto it = _instances.find(handle._id);
        return it->second;
    }

    const Type& get(Handle<Type> const& handle) const {
        
        auto it = _instances.find(handle._id);
        return it->second;
    }

private:

    size_t _offset{0};
    std::unordered_map<uint32_t, Type> _instances;
};

}