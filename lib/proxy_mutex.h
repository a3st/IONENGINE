// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

template<class Type>
class ProxyMutex {
public:

    ProxyMutex(Type* const ptr, std::mutex& mutex) : 
        _ptr(ptr), _lock(mutex) { }

    Type* operator->() { 
        return _ptr;
    }
        
    Type const* operator->() const {
        return _ptr;
    }

private:

    Type* const _ptr;
    std::unique_lock<std::mutex> _lock;
};

}
