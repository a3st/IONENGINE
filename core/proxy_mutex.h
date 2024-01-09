// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

template<class Type, class Mutex = std::shared_mutex, class Lock = std::unique_lock<Mutex>>
class ProxyMutex {
public:

    ProxyMutex(Type* const ptr, Mutex& mutex) : 
        _ptr(ptr), _lock(mutex) { }

    Type* operator->() { 
        return _ptr;
    }
        
    Type const* operator->() const {
        return _ptr;
    }

private:

    Type* const _ptr;
    Lock _lock;
};

template<class Type, class Mutex>
class ProxyMutex<Type, Mutex, std::shared_lock<Mutex>> {
public:

    ProxyMutex(Type* const ptr, Mutex& mutex) : 
        _ptr(ptr), _lock(mutex) { }

    Type const* operator->() const {
        return _ptr;
    }

private:

    Type* const _ptr;
    std::shared_lock<Mutex> _lock;
};

}
