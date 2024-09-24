// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "shadersys/compiler.hpp"

#pragma once

namespace ionengine::shadersys
{
    class DXCCompiler : public ShaderCompiler
    {
      public:
        DXCCompiler(fx::ShaderAPIType const apiType);
    };
} // namespace ionengine::shadersys