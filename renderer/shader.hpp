// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/device.hpp"

namespace ionengine::renderer
{
    class Shader : public core::ref_counted_object
    {
      public:
        Shader(rhi::Device& device, std::span<uint8_t const> const data_bytes);

        auto get_shader() -> core::ref_ptr<rhi::Shader>
        {
            return shader.get();
        }

      private:
        core::ref_ptr<rhi::Shader> shader;
    };
} // namespace ionengine::renderer