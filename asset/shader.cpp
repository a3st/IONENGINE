// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "shader.hpp"
#include "math/vector.hpp"
#include "precompiled.h"
#include <xxhash/xxhash64.h>

namespace ionengine
{
    Shader::Shader(renderer::Renderer& renderer) : renderer(&renderer)
    {
    }

    auto Shader::load(std::span<uint8_t> const data_bytes) -> bool
    {
        shader = renderer->create_shader(data_bytes);
        return true;
    }

} // namespace ionengine