// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_buffer.h>

namespace ionengine::renderer {

class CBufferPool {
public:

    CBufferPool(backend::Device& device, uint32_t const buffer_size, uint32_t const pool_size) {

        _data.resize(pool_size);
        for(uint32_t i = 0; i < pool_size; ++i) {
            _data.at(i) = GPUBuffer::cbuffer(device, buffer_size);
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
