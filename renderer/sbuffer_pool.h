// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_buffer.h>

namespace ionengine::renderer {

class SBufferPool {
public:

    SBufferPool(backend::Device& device, uint32_t const buffer_size, uint32_t const element_stride, uint32_t const pool_size) {
        _data.resize(pool_size);
        for(uint32_t i = 0; i < pool_size; ++i) {
            _data.at(i) = GPUBuffer::sbuffer(device, buffer_size, element_stride);
        }
    }

    void reset() {
        _offset = 0;
    }

    std::shared_ptr<GPUBuffer> allocate() {
        auto buffer = _data.at(_offset);
        ++_offset;
        return buffer;
    }

private:

    std::vector<std::shared_ptr<GPUBuffer>> _data;
    uint32_t _offset{0};
};

}
