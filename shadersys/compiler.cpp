// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "compiler.hpp"
#include "precompiled.h"
#ifdef IONENGINE_SHADERSYS_DXC
#include "dxc/dxc.hpp"
#endif

namespace ionengine::shadersys
{
    auto ShaderCompiler::create(asset::fx::ShaderFormat const shaderFormat,
                                std::filesystem::path const& includeBasePath) -> core::ref_ptr<ShaderCompiler>
    {
#ifdef IONENGINE_SHADERSYS_DXC
        return core::make_ref<DXCCompiler>(shaderFormat, includeBasePath);
#else
#error shader system backend is not defined
#endif
    }
} // namespace ionengine::shadersys