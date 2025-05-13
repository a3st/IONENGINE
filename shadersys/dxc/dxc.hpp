// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../compiler.hpp"
#define NOMINMAX
#include <d3d12shader.h>
#include <dxcapi.h>
#include <dxgi1_4.h>
#include <windows.h>
#include <winrt/base.h>

namespace ionengine::shadersys
{
    class DXCCompiler : public ShaderCompiler
    {
      public:
        DXCCompiler(asset::fx::ShaderFormat const shaderFormat);

        auto compile(std::filesystem::path const& filePath) -> std::expected<asset::ShaderFile, core::error> override;

      private:
        winrt::com_ptr<IDxcCompiler3> compiler;
        winrt::com_ptr<IDxcUtils> utils;
        winrt::com_ptr<IDxcIncludeHandler> includeHandler;
        asset::fx::ShaderFormat shaderFormat;

        auto getOutputStates(std::unordered_map<std::string, std::string> const& attributes,
                             asset::fx::OutputData& outputData) -> void;

        auto getEffectDataCode(asset::fx::StructureData const& structureData) -> std::string;

        auto isDefaultSemantic(std::string_view const semantic) -> bool;
    };
} // namespace ionengine::shadersys