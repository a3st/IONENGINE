// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "shadersys/fx.hpp"

namespace ionengine::shadersys
{
    class ShaderCompiler : public core::ref_counted_object
    {
      public:
        virtual ~ShaderCompiler() = default;

        static auto create(fx::ShaderAPIType const apiType) -> core::ref_ptr<ShaderCompiler>;

        virtual auto compileFromBytes(std::span<uint8_t const> const dataBytes) -> std::optional<fx::ShaderEffectFile> = 0;

        virtual auto compileFromFile(std::filesystem::path const& filePath) -> std::optional<fx::ShaderEffectFile> = 0;
    };
} // namespace ionengine::shadersys