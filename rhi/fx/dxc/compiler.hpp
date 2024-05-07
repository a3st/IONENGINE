// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/fx/compiler.hpp"
#include "rhi/fx/fxsl.hpp"
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

#pragma comment(lib, "dxcompiler.lib")
#pragma comment(lib, "dxgi.lib")

namespace ionengine::rhi::fx
{
    class DXCCompiler : public FXSLCompiler
    {
      public:
        DXCCompiler();

        auto add_include_path(std::filesystem::path const& include_path) -> void override;

        auto compile(std::filesystem::path const& file_path) -> std::optional<std::vector<uint8_t>> override;

      private:
        winrt::com_ptr<IDxcCompiler3> compiler;
        std::vector<std::filesystem::path> include_paths;

        std::set<std::string> input_assembler_names;

        auto merge_shader_code(std::string& shader_code) -> bool;

        auto convert_shader_constants(std::string& shader_code, std::vector<ShaderConstantData>& constants,
                                      std::span<ShaderStructureData const> const structures) -> bool;

        auto convert_shader_structures(std::string& shader_code, std::vector<ShaderStructureData>& structures) -> bool;

        auto convert_bindless_constants(std::string& shader_code, std::span<ShaderConstantData const> const constants,
                                        std::span<ShaderStructureData const> const structures,
                                        ShaderTechniqueData const& technique) -> bool;

        auto convert_shader_technique(std::string& shader_code, ShaderTechniqueData& technique) -> bool;
    };
} // namespace ionengine::rhi::fx