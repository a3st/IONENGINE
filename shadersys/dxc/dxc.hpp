// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

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
        DXCCompiler(asset::fx::ShaderFormat const shaderFormat, std::filesystem::path const& includeBasePath);

        auto compile(std::filesystem::path const& filePath) -> std::expected<asset::ShaderFile, core::error> override;

      private:
        winrt::com_ptr<IDxcCompiler3> _compiler;
        winrt::com_ptr<IDxcUtils> _utils;
        winrt::com_ptr<IDxcIncludeHandler> _includeHandler;
        asset::fx::ShaderFormat _shaderFormat;
        std::filesystem::path _includeBasePath;
        std::unordered_map<uint64_t, uint32_t> _cacheShaderStages;

        auto getOutputStates(std::unordered_map<std::string, std::string> const& attributes,
                             asset::fx::OutputData& output) const -> void;

        auto getDomainStructures(asset::fx::HeaderData const& header, asset::fx::StructureData const& effectStructure,
                                 std::span<std::string const> const permutationNames, std::string& shaderCode,
                                 std::vector<asset::fx::StructureData>& structures) const -> bool;

        auto getPermutations(asset::fx::HeaderData const& header,
                             std::unordered_map<uint32_t, std::string>& permutationNames,
                             std::unordered_set<uint32_t>& permutationMasks) const -> void;

        auto getPermutationNamesByMask(std::unordered_map<uint32_t, std::string> const& permutationNames,
                                       uint32_t const mask, std::vector<std::string>& outputNames) const -> void;
    };
} // namespace ionengine::shadersys