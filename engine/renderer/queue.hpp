// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "surface.hpp"
#include "material.hpp"

namespace ionengine
{
    class RenderQueue
    {
      public:
        RenderQueue()
        {
        }

        auto push(core::ref_ptr<Surface> surface, core::ref_ptr<Material> material, uint16_t const layer) -> void
        {
            elements.emplace_back(Element{.surface = surface, .material = material, .layer = layer});
        }

      private:
        struct Element
        {
            core::ref_ptr<Surface> surface;
            core::ref_ptr<Material> material;
            uint16_t layer;
        };

        std::vector<Element> elements;
        std::vector<size_t> indices;
    };
} // namespace ionengine