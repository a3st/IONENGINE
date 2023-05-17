// Copyright © 2020-2023 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ie {

namespace core {

class ref_counted_object {
public:

    ref_counted_object() : ref_count(0) {
        std::cout << "Construct object" << std::endl;
    }

    ~ref_counted_object() {
        std::cout << "Cleanup object" << std::endl;
    }

    ref_counted_object(ref_counted_object& other) : ref_count(other.ref_count.load()) { }

    ref_counted_object(ref_counted_object&& other) : ref_count(other.ref_count.load()) { }

    auto operator=(ref_counted_object& other) -> ref_counted_object& {
        ref_count = other.ref_count.load();
        return *this;
    }
    
    auto operator=(ref_counted_object&& other) -> ref_counted_object& {
        ref_count = other.ref_count.load();
        return *this;
    }

    auto add_ref() -> uint32_t {
        return ++ref_count;
    }

    auto release() -> uint32_t {
        return --ref_count;
    }

private:

    std::atomic<uint32_t> ref_count;
};

template<typename Type>
class ref_ptr {
public:

    ref_ptr(std::nullptr_t) : ptr(nullptr) { }

    ~ref_ptr() {
        uint32_t count = ptr->release();
        if(count == 0) {
            delete ptr;
        }
    }

    ref_ptr(Type* ptr_) : ptr(ptr_) {
        if(ptr != nullptr) {
            ptr->add_ref();
        }
    }

    ref_ptr(ref_ptr const& other) : ptr(other.ptr) {
        if(ptr != nullptr) {
            ptr->add_ref();
        }
    }

    ref_ptr(ref_ptr&& other) : ptr(other.ptr) { }

    auto operator=(ref_ptr const& other) -> ref_ptr& {
        ptr = other.ptr;
        if(ptr != nullptr) {
            ptr->add_ref();
        }
        return *this;
    }

    auto operator=(ref_ptr&& other) -> ref_ptr& { 
        ptr = other.ptr;
        return *this;
    }

    auto operator->() -> Type* {
        assert(ptr != nullptr && "ref_ptr is null");
        return ptr;
    }

    auto operator&() -> Type& {
        assert(ptr != nullptr && "ref_ptr is null");
        return *ptr;
    }

    operator bool() const {
        return ptr != nullptr;
    }
    
private:

    Type* ptr;
};

template <typename Type>
auto operator!=(std::nullptr_t, ref_ptr<Type> const& rhs) -> bool {
    return !(nullptr == rhs);
}

template<typename Type, typename ...Args>
inline auto make_ref(Args&& ...args) -> ref_ptr<Type> {
    Type* ptr = new Type(std::forward<Args>(args)...);
    return ref_ptr<Type>(ptr);
}

}

}
