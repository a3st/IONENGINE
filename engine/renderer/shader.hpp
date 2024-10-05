// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"
#include "shadersys/fx.hpp"

namespace ionengine
{
    class Shader : public core::ref_counted_object
    {
      public:
        /*struct ShaderOption
        {
            uint32_t constantIndex;
            rhi::fx::ShaderElementType elementType;
            uint64_t offset;
            size_t size;
        };*/

        Shader(rhi::Device& device, shadersys::fx::ShaderEffectFile const& shaderEffect);

      private:
        core::ref_ptr<rhi::Shader> shaderProgram;

        rhi::RasterizerStageInfo rasterizerStageInfo;
    };
} // namespace ionengine