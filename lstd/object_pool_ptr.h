// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "object_pool.h"

namespace lstd {

template<class T>
inline void pooled_object_deleter(object_pool<T>* pool, T* ptr) {

    pool->release(ptr);
}

template<class T>
using unique_object_ptr = std::unique_ptr<T, std::function<void(T*)>>;

template<class T>
inline unique_object_ptr<T> make_unique_object_ptr(object_pool<T>* pool) {
    
    return unique_object_ptr<T>(pool->acquire(), [pool](T* ptr) { pooled_object_deleter(pool, ptr); });
}

}