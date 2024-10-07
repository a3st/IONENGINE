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

        static auto create(fx::APIType const apiType) -> core::ref_ptr<ShaderCompiler>;

        virtual auto compileFromFile(std::filesystem::path const& filePath, std::string& errors) -> std::optional<ShaderEffectFile> = 0;
    };
} // namespace ionengine::shadersys