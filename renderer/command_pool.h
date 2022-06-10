// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/resource_ptr.h>
#include <renderer/command_list.h>

namespace ionengine::renderer {

enum class CommandPoolType {
    Graphics,
    Copy,
    Compute,
    Bundle
};

template<CommandPoolType PoolType>
class CommandPool { 
public:

    CommandPool(backend::Device& device, uint32_t const pool_size);

    void reset();

    ResourcePtr<CommandList> allocate();
};

template<>
class CommandPool<CommandPoolType::Graphics> {
public:

    CommandPool(backend::Device& device, uint32_t const pool_size) {
        for(uint32_t i = 0; i < pool_size; ++i) {
            _data.emplace_back(make_resource_ptr(CommandList::create(device, backend::QueueFlags::Graphics).value()));
        }
    }

    CommandPool(CommandPool&& other) noexcept {
        _data = std::move(other._data);
        _offset = std::move(other._offset);
    }

    CommandPool& operator=(CommandPool&& other) noexcept {
        _data = std::move(other._data);
        _offset = std::move(other._offset);
    }

    void reset() {
        _offset = 0;
    }

    ResourcePtr<CommandList> allocate() {
        auto command_list = _data.at(_offset);
        ++_offset;
        return command_list;
    }

private:

    std::vector<ResourcePtr<CommandList>> _data;
    uint32_t _offset{0};
};

template<>
class CommandPool<CommandPoolType::Bundle> {
public:

    CommandPool(backend::Device& device, uint32_t const pool_size) {
        for(uint32_t i = 0; i < pool_size; ++i) {
            _data.emplace_back(make_resource_ptr(CommandList::create(device, backend::QueueFlags::Graphics, true).value()));
        }
    }

    CommandPool(CommandPool&& other) noexcept {
        _data = std::move(other._data);
        _offset = std::move(other._offset);
    }

    CommandPool& operator=(CommandPool&& other) noexcept {
        _data = std::move(other._data);
        _offset = std::move(other._offset);
    }

    void reset() {
        _offset = 0;
    }

    ResourcePtr<CommandList> allocate() {
        auto command_list = _data.at(_offset);
        ++_offset;
        return command_list;
    }

private:

    std::vector<ResourcePtr<CommandList>> _data;
    uint32_t _offset{0};
};

template<>
class CommandPool<CommandPoolType::Compute> {
public:

    CommandPool(backend::Device& device, uint32_t const pool_size) {
        for(uint32_t i = 0; i < pool_size; ++i) {
            _data.emplace_back(make_resource_ptr(CommandList::create(device, backend::QueueFlags::Compute).value()));
        }
    }

    CommandPool(CommandPool&& other) noexcept {
        _data = std::move(other._data);
        _offset = std::move(other._offset);
    }

    CommandPool& operator=(CommandPool&& other) noexcept {
        _data = std::move(other._data);
        _offset = std::move(other._offset);
    }

    void reset() {
        _offset = 0;
    }

    ResourcePtr<CommandList> allocate() {
        std::unique_lock lock(_mutex);
        auto command_list = _data.at(_offset);
        ++_offset;
        return command_list;
    }

private:

    std::vector<ResourcePtr<CommandList>> _data;

    std::mutex _mutex;

    uint32_t _offset{0};
};

}
