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

        auto add_include_path(std::filesystem::path const& include_path) -> void;

        auto compile(std::filesystem::path const& file_path) -> void;

      private:
        winrt::com_ptr<IDxcCompiler3> compiler;
        std::vector<std::filesystem::path> include_paths;

        auto merge_shader_code(std::filesystem::path const& file_path) -> std::string;

        auto get_shader_constants(std::string const& shader_code) -> std::vector<ShaderConstantData>;

        auto parse_shader_resources(std::string const& shader_code) -> std::set<std::string>;

        std::set<std::string> input_assembler_names;
    };
} // namespace ionengine::rhi::fx