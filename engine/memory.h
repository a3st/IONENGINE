// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/pool_allocator.h>

namespace ionengine {

class SceneNode;

template<class Type>
struct Memory {
    inline static PoolAllocator<4096, 64> allocator;
};

template<>
struct Memory<SceneNode> {
    inline static PoolAllocator<1024 * 1024, 128> allocator;
};

template <class Type>
class Allocator {
public:

    typedef Type value_type;
    typedef Type* pointer;
    typedef const Type* const_pointer;
    typedef Type& reference;
    typedef const Type& const_reference;
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    template <class U>
    struct rebind {
        typedef Allocator<U> other;
    };

    Allocator() throw() { }

    Allocator(Allocator const&) throw() { }

    template <class U>
    Allocator(Allocator<U> const&) throw() { }

    ~Allocator() throw() { }

    pointer address(reference x) const { return &x; }

    const_pointer address(const_reference x) const { return &x; }

    pointer allocate(size_type n, const_pointer cp = 0) {

        return reinterpret_cast<Type*>(Memory<Type>::allocator.allocate(n, 0));
    }

    void deallocate(pointer p, size_type n) {

        Memory<Type>::allocator.deallocate(p);
    }

    size_type max_size() const throw() {

        return Memory<Type>::allocator.max_size();
    }

    void construct(pointer p, value_type const& x) {

        ::new ((void*)p) value_type(x);
    }

    void destroy(pointer p) {

        p->~Type();
    }
};

}