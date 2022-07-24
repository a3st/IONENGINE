// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <core/include/core/allocator.hpp>

namespace ionengine::core {

inline size_t constexpr VECTOR_SHRINK_SIZE = 2;

template <typename Type>
class Vector {
 public:
    Vector(Vector& other) {
        _data = other._data;
        _allocator = other._allocator;
        _size = other._size;
        _capacity = other._capacity;
    }

    Vector(Vector&& other) noexcept {
        _data = other._data;
        _allocator = other._allocator;
        _size = other._size;
        _capacity = other._capacity;
    }

    Vector& operator=(Vector& other) {
        _data = other._data;
        _allocator = other._allocator;
        _size = other._size;
        _capacity = other._capacity;
        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        _data = other._data;
        _allocator = other._allocator;
        _size = other._size;
        _capacity = other._capacity;
        return *this;
    }

    void push(Type&& element) {
        std::cout << "capacity: " << _capacity << std::endl;
        if (_size >= _capacity) {
            _capacity =
                _size == 0 ? VECTOR_SHRINK_SIZE : _size * VECTOR_SHRINK_SIZE;
            _reallocate(_capacity);
        }
        _assign(element);
    }

    void push(Type const& element) {
        std::cout << "capacity: " << _capacity << std::endl;
        if (_size >= _capacity) {
            _capacity =
                _size == 0 ? VECTOR_SHRINK_SIZE : _size * VECTOR_SHRINK_SIZE;
            _reallocate(_capacity);
        }
        _assign(element);
    }

    bool pop(Type& element) { return false; }

    void clear() {}

    inline bool is_empty() const { return _size == 0; }

    void reserve(size_t const capacity) { _reallocate(capacity); }

    void resize(size_t const size) { _size = size; }

    inline size_t size() const { return _size; }

    inline Type& operator[](size_t const index) {
        assert((index <= _size || index >= 0) && "core::Vector out of index");
        return _data[index];
    }

    inline Type const& operator[](size_t const index) const {
        assert((index <= _size || index >= 0) && "core::Vector out of index");
        return _data[index];
    }

 private:
    Vector(Allocator* allocator)
        : _allocator(allocator), _size(0), _capacity(0), _data(nullptr) {}

    Type* _data;
    Allocator* _allocator;
    size_t _size;
    size_t _capacity;

    void _reallocate(size_t const size) {
        Type* realloc = _allocator
                            ? reinterpret_cast<Type*>(
                                  _allocator->allocate(sizeof(Type) * size))
                            : new Type[size];
        if (!_data) {
            _data = realloc;
        } else {
            std::memmove(realloc, _data, sizeof(Type) * _size);
            if (!_allocator) {
                delete _data;
            } else {
                _allocator->deallocate(_data);
            }
            _data = realloc;
        }
    }

    void _assign(Type&& element) {
        _data[_size] = std::move(element);
        ++_size;
    }

    void _assign(Type const& element) {
        _data[_size] = element;
        ++_size;
    }

    friend Vector make_vector<Type>(Allocator* allocator);
};

template <typename Type>
inline Vector<Type> make_vector(Allocator* allocator = nullptr) {
    return Vector<Type>(allocator);
}

}  // namespace ionengine::core