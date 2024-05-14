// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine::tools::shaderc
{
    class ShaderCompiler
    {
      public:
        virtual auto getTarget() const -> std::string_view = 0;

        virtual auto compile(std::string_view const shaderCode, std::filesystem::path const& outputPath,
                             std::string& errors) -> bool = 0;
    };

    class DXC : public ShaderCompiler
    {
      public:
        DXC();

        auto getTarget() const -> std::string_view override;

        auto compile(std::string_view const shaderCode, std::filesystem::path const& outputPath,
                     std::string& errors) -> bool override;

      private:
        winrt::com_ptr<IDxcCompiler3> compiler;
    };
} // namespace ionengine::tools::shaderc