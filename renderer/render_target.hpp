// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/device.hpp"

namespace ionengine {

namespace renderer {

class RenderTarget : public core::ref_counted_object {
public:

    RenderTarget(rhi::Device& device, core::ref_ptr<rhi::MemoryAllocator> allocator, uint32_t const width, uint32_t const height);

    auto get_next_buffer() -> core::ref_ptr<rhi::Texture> {

        buffer_index = (buffer_index + 1) % static_cast<uint32_t>(buffers.size());
        return buffers[buffer_index];
    }

private:

    std::vector<core::ref_ptr<rhi::Texture>> buffers;
    uint32_t buffer_index{0};
};

}

}