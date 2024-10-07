// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "shadersys/compiler.hpp"
#define NOMINMAX
#include <windows.h>
#include <d3d12shader.h>
#include <dxcapi.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine::shadersys
{
    class DXCCompiler : public ShaderCompiler
    {
      public:
        DXCCompiler(fx::APIType const apiType);

        auto compileFromFile(std::filesystem::path const& filePath, std::string& errors) -> std::optional<ShaderEffectFile> override;

      private:
        winrt::com_ptr<IDxcCompiler3> compiler;
        winrt::com_ptr<IDxcUtils> utils;
        winrt::com_ptr<IDxcIncludeHandler> includeHandler;

        fx::APIType apiType;
    };
} // namespace ionengine::shadersys