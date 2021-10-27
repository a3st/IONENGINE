// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace lstd {

template<class T>
class object_pool {

public:

    object_pool() {
  
    }

    template<class... Args>
    object_pool(const size_t size, Args&&... args) : size_(size) {

        for(uint32_t i = 0; i < size; ++i) {
            objects_.push(new T(std::forward<Args>(args)...));
        }
    }

    object_pool(const object_pool&) = delete;
   
    object_pool(object_pool&& rhs) noexcept {
        
        std::swap(objects_, rhs.objects_);
        std::swap(size_, rhs.size_);
    }

    object_pool& operator=(const object_pool&) = delete;

    object_pool& operator=(object_pool&& rhs) noexcept {

        std::swap(objects_, rhs.objects_);
        std::swap(size_, rhs.size_);
        return *this;
    }

    T* construct() {

        std::lock_guard lock(mutex_);

        if(objects_.empty()) {
            throw std::runtime_error("object_pool acquire is error");
        }

        T* ret = objects_.top();
        objects_.pop();
        return ret;
    }

    void destroy(T* object) {

        std::lock_guard lock(mutex_);

        objects_.push(object);
    }

    inline size_t size() const { return size_; }

private:

    std::stack<T*> objects_;
    size_t size_;

    std::mutex mutex_;
};

template<class T>
inline void pooled_object_deleter(object_pool<T>* pool, T* ptr) {

    pool->destroy(ptr);
}

template<class T>
using unique_object_ptr = std::unique_ptr<T, std::function<void(T*)>>;

template<class T>
inline unique_object_ptr<T> make_unique_object_ptr(object_pool<T>* pool) {
    
    return unique_object_ptr<T>(pool->construct(), [pool](T* ptr) { pooled_object_deleter(pool, ptr); });
}

}