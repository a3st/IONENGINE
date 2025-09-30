// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"
#include "core/ref_ptr.hpp"
#include "fx.hpp"

namespace ionengine::shadersys
{
    class ShaderCompiler : public core::ref_counted_object
    {
      public:
        virtual ~ShaderCompiler() = default;

        static auto create(asset::fx::ShaderFormat const shaderFormat, std::filesystem::path const& includeBasePath)
            -> core::ref_ptr<ShaderCompiler>;

        virtual auto compile(std::filesystem::path const& filePath)
            -> std::expected<asset::ShaderFile, core::error> = 0;
    };
} // namespace ionengine::shadersys