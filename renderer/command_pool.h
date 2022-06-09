// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/resource_ptr.h>
#include <renderer/command_list.h>

namespace ionengine::renderer {

enum class CommandPoolType {
    Graphics,
    Copy,
    Compute
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
            _data.emplace_back(CommandList::create(device, backend::QueueFlags::Graphics).value());
        }
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
            _data.emplace_back(CommandList::create(device, backend::QueueFlags::Compute).value());
        }
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

}
