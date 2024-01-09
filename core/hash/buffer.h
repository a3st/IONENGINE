// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <xxhash/xxhash64.h>

namespace ionengine::lib::hash {

template<class Type>
class Buffer {
public:

    Buffer() = default;

    Buffer(std::span<Type const> const data) {
        _data.resize(data.size());
        std::memcpy(_data.data(), data.data(), data.size_bytes());
        _data_hash = XXHash64::hash(reinterpret_cast<void*>(_data.data()), _data.size() * sizeof(Type), 0);
    }

    Buffer(Buffer const& other) {
        _data = other._data;
        _data_hash = other._data_hash;
    }

    Buffer(Buffer&& other) noexcept {
        _data = std::move(other._data);
        _data_hash = other._data_hash;
    }

    Buffer& operator=(Buffer const& other) {
        _data = other._data;
        _data_hash = other._data_hash;
        return *this;
    }

    Buffer& operator=(Buffer&& other) noexcept {
        _data = std::move(other._data);
        _data_hash = other._data_hash;
        return *this;
    }

    uint64_t hash() const { 
        return _data_hash;
    }

    Type const* data() const {
        return _data.data();
    }

    size_t size() const {
        return _data.size();
    }

    std::span<Type const> to_span() {
        return std::span<Type const>(_data.data(), _data.size());
    }

private:
    
    std::vector<Type> _data;
    uint64_t _data_hash{0};
};

}
