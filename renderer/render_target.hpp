// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/device.hpp"

namespace ionengine::renderer
{
    class RenderTarget : public core::ref_counted_object
    {
      public:
        RenderTarget(rhi::Device& device, core::ref_ptr<rhi::MemoryAllocator> allocator, uint32_t const width,
                     uint32_t const height);

        auto get_buffer(uint32_t const index) -> core::ref_ptr<rhi::Texture>
        {
            return buffers[index];
        }

        auto get_width() const -> uint32_t
        {
            return width;
        }

        auto get_height() const -> uint32_t
        {
            return height;
        }

      private:
        std::vector<core::ref_ptr<rhi::Texture>> buffers;
        uint32_t width;
        uint32_t height;
    };
} // namespace ionengine::renderer