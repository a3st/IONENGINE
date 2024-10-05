// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "renderer.hpp"
#include "precompiled.h"

namespace ionengine
{
    Renderer::Renderer(rhi::Device& device) : device(&device)
    {
    }

    auto createShader(shadersys::fx::ShaderEffectFile const& shaderEffect) -> core::ref_ptr<Shader>
    {
        return nullptr;
    }
} // namespace ionengine