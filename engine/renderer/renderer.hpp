// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "linked_device.hpp"
#include "texture.hpp"
#include "shader.hpp"

namespace ionengine
{
    class Renderer
    {
      public:
        Renderer(LinkedDevice& device);

        auto registerShader(std::string_view const shaderName, core::ref_ptr<ShaderAsset> shaderAsset) -> bool;

        /*auto add_point_light() -> void
        {
        }

        auto add_directional_light() -> void
        {
        }

        auto add_spot_light() -> void
        {
        }

        auto draw_mesh(core::ref_ptr<Mesh> mesh) -> void;

        auto draw_quad(math::Matrixf const& view_proj) -> void;

        auto begin_draw(std::span<core::ref_ptr<Texture> const> const colors, core::ref_ptr<Texture> depth_stencil,
                        math::Color const& clear_color, float const clear_depth, uint8_t const clear_stencil) -> void;

        auto end_draw() -> void;*/

      private:
        LinkedDevice* device;
    };
} // namespace ionengine