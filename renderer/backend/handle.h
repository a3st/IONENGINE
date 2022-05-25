// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/exception.h>

namespace ionengine::renderer::backend {

template<class Type>
class Handle {
public:

    Handle() = default;

    Handle(uint32_t const index, uint32_t const generation) : _index(index), _generation(generation) { }

    Handle(Handle const& other) : _index(other._index), _generation(other._generation) { }
    
    Handle(Handle&& other) noexcept : _index(other._index), _generation(other._generation) { }

    Handle& operator=(Handle const& other) {

        _index = other._index;
        _generation = other._generation;
        return *this;
    }

    Handle& operator=(Handle&& other) noexcept {

        _index = other._index;
        _generation = other._generation;
        return *this;
    }

    bool operator==(Handle const& other) const { return std::tie(_index, _generation) == std::tie(other._index, other._generation); }

    bool operator!=(Handle const& other) const { return std::tie(_index, _generation) != std::tie(other._index, other._generation); }

    uint32_t index() const { return _index; }
    
    uint32_t generation() const { return _generation; }

private:

    uint32_t _index{std::numeric_limits<uint32_t>::max()};
    uint32_t _generation{0};
};

template<class Type>
using InvalidHandle = Handle<Type>;

template<class Type>
class HandlePool {
private:

    struct HandleData {
        std::optional<Type> data;
        uint32_t generation;
    };

public:

    HandlePool() = default;

    HandlePool(HandlePool const& other) = delete;

    HandlePool(HandlePool&& other) noexcept {

        _data = other._data;
        _free_data = other._free_data;
    }

    HandlePool& operator=(HandlePool const& other) noexcept = delete;

    HandlePool& operator=(HandlePool&& other) noexcept {

        _data = other._data;
        _free_data = other._free_data;
        return *this;
    }

    HandlePool(uint32_t const pool_size) {

        _data.resize(pool_size);
        _free_data.resize(pool_size);

        fill_free_empty_data(pool_size);
    }

    Handle<Type> push(Type const& element) {
        
        assert(!_free_data.empty() && "the pool is already full");

        uint32_t const index = _free_data.back();
        _free_data.pop_back();

        auto handle_data = HandleData {
            .data = element,
            .generation = _data[index].generation + 1 % std::numeric_limits<uint32_t>::max()
        };

        Handle<Type> handle(index, handle_data.generation);

        _data[index] = std::move(handle_data);
        return handle;
    }

    Handle<Type> push(Type&& element) {

        assert(!_free_data.empty() && "the pool is already full");

        uint32_t const index = _free_data.back();
        _free_data.pop_back();

        auto handle_data = HandleData {
            .data = std::move(element),
            .generation = _data[index].generation + 1 % std::numeric_limits<uint32_t>::max()
        };

        Handle<Type> handle(index, handle_data.generation);

        _data[index] = std::move(handle_data);
        return handle;
    }

    void erase(Handle<Type> const& handle) {

        _data[handle.index()].data.reset();
        _free_data.push_back(handle.index());
    }

    void clear() {

        uint32_t const pool_size = static_cast<uint32_t>(_data.size());

        _data.resize(pool_size);
        _free_data.resize(pool_size);

        fill_free_empty_data(pool_size);
    }

    Type const& operator[](Handle<Type> const& handle) const {

        if(_data.at(handle.index()).generation != handle.generation()) {
            throw ionengine::lib::Exception(std::format("Could not find required handle (id: {}, generation: {})", handle.index(), handle.generation()));
        }
        return _data.at(handle.index()).data.value();
    }

    Type& operator[](Handle<Type> const& handle) {

        if(_data[handle.index()].generation != handle.generation()) {
            throw ionengine::lib::Exception(std::format("Could not find required handle (id: {}, generation: {})", handle.index(), handle.generation()));
        }
        return _data[handle.index()].data.value();
    }

private:

    std::vector<HandleData> _data;
    std::vector<uint32_t> _free_data;

    void fill_free_empty_data(uint32_t const fill_size) {

        std::iota(_free_data.begin(), _free_data.end(), 0);
    }
};

}
