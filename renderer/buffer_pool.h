// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/resource_ptr.h>
#include <renderer/gpu_buffer.h>

namespace ionengine::renderer {

enum class BufferPoolType {
    SBuffer,
    CBuffer,
    RWBuffer
};

enum class BufferPoolUsage {
    Dynamic,
    Static
};

template<BufferPoolType PoolType, size_t DataSize>
class BufferPool { 
public:

    BufferPool(backend::Device& device, uint32_t const pool_size, BufferPoolUsage const usage);

    void reset();

    ResourcePtr<GPUBuffer> allocate();
};

template<size_t DataSize>
class BufferPool<BufferPoolType::CBuffer, DataSize> {
public:

    BufferPool(backend::Device& device, uint32_t const pool_size, BufferPoolUsage const usage) {
        for(uint32_t i = 0; i < pool_size; ++i) {
            if(usage == BufferPoolUsage::Dynamic) {
                _data.emplace_back(GPUBuffer::create(device, DataSize, backend::BufferFlags::ConstantBuffer | backend::BufferFlags::HostWrite).value());
            } else {
                _data.emplace_back(GPUBuffer::create(device, DataSize, backend::BufferFlags::ConstantBuffer).value());
            }
        }
    }

    BufferPool(BufferPool&& other) noexcept {
        _data = std::move(other._data);
        _offset = std::move(other._offset);
    }

    BufferPool& operator=(BufferPool&& other) noexcept {
        _data = std::move(other._data);
        _offset = std::move(other._offset);
    }

    void reset() {
        _offset = 0;
    }

    ResourcePtr<GPUBuffer> allocate() {
        std::unique_lock lock(_mutex);
        auto buffer = _data.at(_offset);
        ++_offset;
        return buffer;
    }

private:

    std::vector<ResourcePtr<GPUBuffer>> _data;

    std::mutex _mutex;

    uint32_t _offset{0};
};

template<size_t DataSize>
class BufferPool<BufferPoolType::SBuffer, DataSize> {
public:

    BufferPool(backend::Device& device, uint32_t const element_count, uint32_t const pool_size, BufferPoolUsage const usage) {
        for(uint32_t i = 0; i < pool_size; ++i) {
            if(usage == BufferPoolUsage::Dynamic) {
                _data.emplace_back(GPUBuffer::create(device, DataSize * element_count, backend::BufferFlags::ShaderResource | backend::BufferFlags::HostWrite, DataSize).value());
            } else {
                _data.emplace_back(GPUBuffer::create(device, DataSize * element_count, backend::BufferFlags::ShaderResource, DataSize).value());
            }
        }
    }

    BufferPool(BufferPool&& other) noexcept {
        _data = std::move(other._data);
        _offset = std::move(other._offset);
    }

    BufferPool& operator=(BufferPool&& other) noexcept {
        _data = std::move(other._data);
        _offset = std::move(other._offset);
    }

    void reset() {
        _offset = 0;
    }

    ResourcePtr<GPUBuffer> allocate() {
        std::unique_lock lock(_mutex);
        auto buffer = _data.at(_offset);
        ++_offset;
        return buffer;
    }

private:

    std::vector<ResourcePtr<GPUBuffer>> _data;

    std::mutex _mutex;

    uint32_t _offset{0};
};

}
