// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <xxhash/xxhash64.h>

namespace ionengine::lib {

template <class Type>
class HashBuffer {
 public:
    HashBuffer() = default;

    HashBuffer(Type const* const data, size_t const size) {
        _data = std::vector<Type>(size * sizeof(Type));
        std::memcpy(_data.data(), data, size * sizeof(Type));
        _hash = XXHash64::hash(data, size * sizeof(Type), 0);
    }

    HashBuffer(HashBuffer const& other) {
        _data = other._data;
        _hash = other._data_hash;
    }

    HashBuffer(HashBuffer&& other) noexcept {
        _data = std::move(other._data);
        _hash = std::move(other._data_hash);
    }

    HashBuffer& operator=(HashBuffer const& other) {
        _data = other._data;
        _hash = other._data_hash;
        return *this;
    }

    HashBuffer& operator=(HashBuffer&& other) noexcept {
        _data = std::move(other._data);
        _hash = std::move(other._data_hash);
        return *this;
    }

    inline uint64_t hash() const { return _hash; }

    inline Type const* data() const { return _data.data(); }

    inline size_t size() const { return _data.size(); }

    inline std::span<Type const> to_span() {
        return std::span<Type const>(_data.data(), _data.size());
    }

 private:
    std::vector<Type> _data;
    uint64_t _hash;
};

}  // namespace ionengine::lib
