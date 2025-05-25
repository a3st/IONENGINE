// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"

namespace ionengine
{
    class Surface : public core::ref_counted_object
    {
      public:
        Surface(core::ref_ptr<rhi::Buffer> const& vertexBuffer, core::ref_ptr<rhi::Buffer> const& indexBuffer,
                uint32_t const indexCount);

        auto draw(rhi::GraphicsContext* context) const -> void;

        auto getVertexBuffer() const -> core::ref_ptr<rhi::Buffer>;

        auto getIndexBuffer() const -> core::ref_ptr<rhi::Buffer>;

      private:
        core::ref_ptr<rhi::Buffer> vertexBuffer;
        core::ref_ptr<rhi::Buffer> indexBuffer;
        uint32_t indexCount;
    };
} // namespace ionengine