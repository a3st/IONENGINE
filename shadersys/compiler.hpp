// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "shadersys/fx.hpp"
#undef EOF

namespace ionengine::shadersys
{
    enum class CompileError : uint32_t
    {
        EOF
    };

    class ShaderCompiler : public core::ref_counted_object
    {
      public:
        virtual ~ShaderCompiler() = default;

        static auto create(asset::fx::ShaderFormat const shaderFormat) -> core::ref_ptr<ShaderCompiler>;

        virtual auto compileFromFile(std::filesystem::path const& filePath, std::string& errors)
            -> std::expected<asset::ShaderFile, CompileError> = 0;
    };
} // namespace ionengine::shadersys