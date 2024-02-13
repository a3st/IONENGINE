// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "asset.hpp"
#include "renderer/renderer.hpp"

namespace ionengine
{
    class Shader : public Asset
    {
      public:
        Shader(renderer::Renderer& renderer);

        auto load(std::span<uint8_t> const data_bytes) -> bool;

        auto get_shader() -> core::ref_ptr<renderer::Shader>
        {
            return shader;
        }

      private:
        renderer::Renderer* renderer;
        core::ref_ptr<renderer::Shader> shader;
    };
} // namespace ionengine