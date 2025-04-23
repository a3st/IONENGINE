// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/matrix.hpp"
#include "core/rect.hpp"
#include "mesh.hpp"
#include "shader.hpp"

namespace ionengine
{
    struct DrawableData
    {
        core::ref_ptr<Surface> surface;
        core::ref_ptr<Shader> shader;
        core::ref_ptr<rhi::Buffer> effectDataBuffer;
        core::ref_ptr<rhi::Buffer> transformDataBuffer;
        std::optional<core::Recti> scissorRect;
    };

    class RenderQueue
    {
      public:
        using ConstIterator = std::vector<DrawableData>::const_iterator;

        using Iterator = std::vector<DrawableData>::iterator;

        RenderQueue();

        auto push(DrawableData&& drawableData) -> void;

        auto clear() -> void;

        ConstIterator begin() const;

        ConstIterator end() const;

        Iterator begin();

        Iterator end();

      private:
        std::vector<DrawableData> elements;
    };
} // namespace ionengine