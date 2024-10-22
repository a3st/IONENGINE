// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"

namespace ionengine
{
    class Surface : public core::ref_counted_object
    {
      public:
        Surface(core::ref_ptr<rhi::Buffer> vertexBuffer, core::ref_ptr<rhi::Buffer> indexBuffer,
                uint32_t const indexCount);

        auto draw(rhi::GraphicsContext& context) -> void;

      private:
        core::ref_ptr<rhi::Buffer> vertexBuffer;
        core::ref_ptr<rhi::Buffer> indexBuffer;
        uint32_t indexCount;
    };
}