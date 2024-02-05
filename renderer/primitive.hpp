// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/device.hpp"

namespace ionengine::renderer
{
    class Primitive : public core::ref_counted_object
    {
      public:
        Primitive(rhi::Device& device, core::ref_ptr<rhi::MemoryAllocator> allocator, uint32_t const index_count,
                  std::span<uint8_t const> const vertices, std::span<uint8_t const> const indices,
                  bool const immediate = false);

        auto is_ready() const -> bool
        {
            return vertices.get_result() && indices.get_result();
        }

        auto vertex_buffer() -> core::ref_ptr<rhi::Buffer>
        {
            return vertices.get();
        }

        auto index_buffer() -> core::ref_ptr<rhi::Buffer>
        {
            return indices.get();
        }

        auto get_index_count() const -> uint32_t
        {
            return index_count;
        }

      private:
        rhi::Future<rhi::Buffer> vertices;
        rhi::Future<rhi::Buffer> indices;
        uint32_t index_count;
    };
} // namespace ionengine::renderer