// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/matrix.hpp"
#include "shader.hpp"

namespace ionengine
{
    struct DrawableData
    {
        // core::ref_ptr<Surface> surface;
        core::ref_ptr<Shader> shader;
        core::Mat4f modelMat;
        uint16_t layerIndex;
    };

    class RenderQueue
    {
      public:
        RenderQueue();

        auto push(DrawableData&& drawableData) -> void;

      private:
        std::vector<DrawableData> rawElements;
    };
} // namespace ionengine