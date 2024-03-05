// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "linked_device.hpp"
#include "math/matrix.hpp"

namespace ionengine
{
    class Mesh;
    class Texture;

    class Renderer
    {
      public:
        Renderer(LinkedDevice& device);

        auto add_point_light() -> void
        {
        }

        auto add_directional_light() -> void
        {
        }

        auto add_spot_light() -> void
        {
        }

        auto draw_mesh(Mesh const& mesh, math::Matrixf const& model, math::Matrixf const& view_proj) -> void;

        auto draw_quad(math::Matrixf const& view_proj) -> void;

        auto begin_draw(std::span<core::ref_ptr<Texture> const> const colors, core::ref_ptr<Texture> depth_stencil,
                        math::Color const& clear_color, float const clear_depth, uint8_t const clear_stencil) -> void;

        auto end_draw() -> void;

      private:
        LinkedDevice* device;
        std::vector<core::ref_ptr<rhi::Texture>> render_pass_color_textures;
    };
} // namespace ionengine