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

        auto merge_shader_code(std::string& shader_code) -> bool;
        
        /*
        auto get_shader_constants(std::string const& shader_code)
            -> std::tuple<std::vector<ShaderConstantData>, std::set<std::string>>;

        auto get_shader_technique(std::string const& shader_code) -> ShaderTechniqueData;

        auto get_shader_structures(std::string const& shader_code, std::set<std::string> const& mappings)
            -> std::vector<ShaderStructureData>;

        auto generate_shader_code_v1(std::string const& shader_code,
                                     std::span<ShaderConstantData const> const constants) -> std::string;

        auto generate_shader_code_v2(std::string const& shader_code,
                                     std::set<std::string> const& mappings) -> std::string;*/

        std::set<std::string> input_assembler_names;
    };
} // namespace ionengine::rhi::fx