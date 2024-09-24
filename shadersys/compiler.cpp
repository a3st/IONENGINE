// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "compiler.hpp"
#include "precompiled.h"

#ifdef IONENGINE_SHADERSYS_DXC
#include "shadersys/dxc/dxc.hpp"
#endif

namespace ionengine::shadersys
{
    auto ShaderCompiler::create(fx::ShaderAPIType const apiType) -> core::ref_ptr<ShaderCompiler>
    {
#ifdef IONENGINE_SHADERSYS_DXC
        return core::make_ref<DXCCompiler>(apiType);
#else
#error shader system backend is not defined
#endif
    }
} // namespace ionengine::shadersys