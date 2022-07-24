// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core {

template <typename Type, size_t Size>
class SmallVector {
 public:
    Array(Array& other) {
        std::memcpy(&_data, &other._data, sizeof(Type) * Size);
    }

    Array(Array&& other) noexcept { 
        std::memmove(&_data, &other._data, sizeof(Type) * Size);
    }

    Array& operator=(Array& other) {
        std::memcpy(&_data, &other._data, sizeof(Type) * Size);
    }

    Array& operator=(Array&& other) noexcept { 
        std::memmove(&_data, &other._data, sizeof(Type) * Size);
        return *this; 
    }

    inline size_t constexpr size() const { return Size; }

    inline Type& operator[](size_t const index) {
        assert((index <= Size || index >= 0) && "core::Array out of index");
        return _data[index]; 
    }

    inline Type const& operator[](size_t const index) const {
        assert((index <= Size || index >= 0) && "core::Array out of index");
        return _data[index];
    }

 private:
    Type _data[Size];
};

}  // namespace ionengine::core