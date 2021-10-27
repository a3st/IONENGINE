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

        for(size_t i = 0; i < size_; ++i) {
            objects_.push(new T(std::forward<Args>(args)...));
        }
    }

    ~object_pool() {

        for(size_t i = 0; i < size_; ++i) {
            T* ret = objects_.top();
            objects_.pop();
            delete ret;
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

        if(objects_.empty()) {
            throw std::runtime_error("object_pool acquire is error");
        }

        T* ret = objects_.top();
        objects_.pop();
        return ret;
    }

    void destroy(T* object) {

        objects_.push(object);
    }

    inline size_t size() const { return size_; }

private:

    std::stack<T*> objects_;
    size_t size_;
};

}