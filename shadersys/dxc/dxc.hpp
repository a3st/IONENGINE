// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "shadersys/compiler.hpp"
#define NOMINMAX
#include <windows.h>
#include <dxcapi.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine::shadersys
{
    class DXCCompiler : public ShaderCompiler
    {
      public:
        DXCCompiler(fx::ShaderAPIType const apiType);

        auto compileFromBytes(std::span<uint8_t const> const dataBytes) -> std::optional<fx::ShaderEffectFile> override;

        auto compileFromFile(std::filesystem::path const& filePath) -> std::optional<fx::ShaderEffectFile> override;

      private:
        winrt::com_ptr<IDxcCompiler3> compiler;

        auto compileBufferData(std::span<uint8_t const> const buffer) -> std::optional<fx::ShaderEffectFile>;
    };
} // namespace ionengine::shadersys