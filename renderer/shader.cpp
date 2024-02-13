// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "shader.hpp"
#include "precompiled.h"

namespace ionengine::renderer
{
    Shader::Shader(rhi::Device& device, std::span<uint8_t const> const data_bytes)
        : shader(device.create_shader(data_bytes))
    {
    }
} // namespace ionengine::renderer