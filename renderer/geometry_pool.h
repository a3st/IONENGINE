// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/resource_ptr.h>
#include <renderer/geometry_buffer.h>

namespace ionengine::renderer {

enum class GeometryPoolUsage {
    Dynamic,
    Static
};

template<size_t VertexSize, size_t IndexSize>
class GeometryPool { 
public:

    GeometryPool(backend::Device& device, uint32_t const pool_size, GeometryPoolUsage const usage) {
        for(uint32_t i = 0; i < pool_size; ++i) {
            if(usage == GeometryPoolUsage::Dynamic) {
                _data.emplace_back(make_resource_ptr(GeometryBuffer::create(device, VertexSize, IndexSize, backend::BufferFlags::VertexBuffer | backend::BufferFlags::IndexBuffer | backend::BufferFlags::HostWrite).value()));
            } else {
                
            }
        }
    }

    GeometryPool(GeometryPool&& other) noexcept {
        _data = std::move(other._data);
        _offset = std::move(other._offset);
    }

    GeometryPool& operator=(GeometryPool&& other) noexcept {
        _data = std::move(other._data);
        _offset = std::move(other._offset);
    }

    void reset() {
        _offset = 0;
    }

    ResourcePtr<GeometryBuffer> allocate() {
        std::unique_lock lock(_mutex);
        auto buffer = _data.at(_offset);
        ++_offset;
        return buffer;
    }

private:

    std::vector<ResourcePtr<GeometryBuffer>> _data;

    std::mutex _mutex;

    uint32_t _offset{0};
};

}
