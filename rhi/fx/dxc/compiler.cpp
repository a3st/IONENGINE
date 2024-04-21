// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "compiler.hpp"
#include "precompiled.h"

namespace ionengine::rhi::fx
{
    namespace hlslconv
    {
        std::unordered_map<std::string, ShaderElementType> types = {
            {"float4x4", ShaderElementType::Float4x4},
            {"float3x3", ShaderElementType::Float3x3},
            {"float2x2", ShaderElementType::Float2x2},
            {"float4", ShaderElementType::Float4},
            {"float3", ShaderElementType::Float3},
            {"float2", ShaderElementType::Float2},
            {"float", ShaderElementType::Float},
            {"bool", ShaderElementType::Bool},
            {"uint", ShaderElementType::Uint},
            {"SamplerState", ShaderElementType::SamplerState},
            {"Texture2D", ShaderElementType::Texture2D}};

        std::unordered_map<std::string, size_t> sizes = {{"float4x4", 64},
                                                         {"float3x3", 36},
                                                         {"float2x2", 16},
                                                         {"float4", 16},
                                                         {"float3", 12},
                                                         {"float2", 8},
                                                         {"float", 4},
                                                         {"bool", 4},
                                                         {"uint", 4},
                                                         {"SamplerState", 4},
                                                         {"Texture2D", 4}};
    } // namespace hlslconv

    DXCCompiler::DXCCompiler()
    {
        input_assembler_names.emplace("VS_INPUT");
        input_assembler_names.emplace("VS_OUTPUT");
        input_assembler_names.emplace("PS_OUTPUT");
    }

    auto DXCCompiler::add_include_path(std::filesystem::path const& include_path) -> void
    {
        include_paths.emplace_back(include_path);
    }

    auto DXCCompiler::compile(std::filesystem::path const& file_path) -> void
    {
        std::string shader_code = merge_shader_code(file_path);

        ShaderTechniqueData technique = get_shader_technique(shader_code);
        auto [constants, mappings] = get_shader_constants(shader_code);
        std::vector<ShaderStructureData> structures = get_shader_structures(shader_code, mappings);

        shader_code = generate_shader_code_v1(shader_code, constants);
        shader_code = generate_shader_code_v2(shader_code, mappings);

        winrt::com_ptr<IDxcCompiler3> compiler;
        ::DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), compiler.put_void());

        DxcBuffer buffer;
        buffer.Ptr = shader_code.data();
        buffer.Size = shader_code.size();
        buffer.Encoding = DXC_CP_ACP;

        std::vector<LPCWSTR> arguments;
        arguments.emplace_back(L"-T vs_6_6");
        arguments.emplace_back(L"-E vs_main");
        arguments.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL3);

        winrt::com_ptr<IDxcResult> result;
        compiler->Compile(&buffer, arguments.data(), static_cast<uint32_t>(arguments.size()), nullptr, __uuidof(IDxcResult),
                        result.put_void());

        winrt::com_ptr<IDxcBlobUtf8> errors;
        result->GetOutput(DXC_OUT_ERRORS, __uuidof(IDxcBlobUtf8), errors.put_void(), nullptr);

        if (errors && errors->GetStringLength() > 0)
        {
            std::cerr << std::string_view(reinterpret_cast<char*>(errors->GetBufferPointer()), errors->GetBufferSize())
                    << std::endl;
        }

        winrt::com_ptr<IDxcBlob> shader;
        result->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), shader.put_void(), nullptr);

        std::cout << shader->GetBufferSize() << std::endl;
    }

    auto DXCCompiler::merge_shader_code(std::filesystem::path const& file_path) -> std::string
    {
        std::string shader_code;
        {
            std::basic_ifstream<char> stream(file_path);
            if (!stream.is_open())
            {
                std::cout << 1 << std::endl;
            }
            stream.seekg(0, std::ios::end);
            uint64_t const offset = stream.tellg();
            shader_code.resize(offset);
            stream.seekg(0, std::ios::beg);
            stream.read(shader_code.data(), shader_code.size());
        }

        std::regex const include_pattern("#include\\s+\"(.+)\"");

        std::smatch match;
        while (std::regex_search(shader_code, match, include_pattern))
        {
            std::string const include_group = match[1].str();

            for (auto const& include_path : include_paths)
            {
                if (std::filesystem::exists(include_path / include_group))
                {
                    std::string include_shader_code;
                    {
                        std::basic_ifstream<char> stream(include_path / include_group);
                        if (!stream.is_open())
                        {
                            std::cout << 1 << std::endl;
                        }
                        stream.seekg(0, std::ios::end);
                        uint64_t const offset = stream.tellg();
                        include_shader_code.resize(offset);
                        stream.seekg(0, std::ios::beg);
                        stream.read(include_shader_code.data(), include_shader_code.size());
                    }

                    auto begin_shader_code = shader_code.substr(0, match.position());
                    auto end_shader_code = shader_code.substr(match.position() + match.length(), std::string::npos);
                    shader_code = begin_shader_code + include_shader_code + end_shader_code;
                }
            }
        }
        return shader_code;
    }

    auto DXCCompiler::get_shader_constants(std::string const& shader_code)
        -> std::tuple<std::vector<ShaderConstantData>, std::set<std::string>>
    {
        std::vector<ShaderConstantData> constants;
        std::set<std::string> mappings;

        std::regex const export_pattern("export\\s+(\\w+)\\s+(\\w+\\[\\]|\\w+)");

        for (auto it = std::sregex_iterator(shader_code.begin(), shader_code.end(), export_pattern);
             it != std::sregex_iterator(); ++it)
        {
            std::smatch const match = *it;

            std::string const export_type_group = match[1].str();
            std::string const struct_name_group = match[2].str();

            ShaderElementType constant_type;
            std::string constant_name = struct_name_group;

            if (hlslconv::types.find(export_type_group) != hlslconv::types.end())
            {
                constant_type = hlslconv::types[export_type_group];
            }
            else
            {
                if (struct_name_group.ends_with("[]"))
                {
                    constant_type = ShaderElementType::StorageBuffer;
                    constant_name = struct_name_group.substr(0, struct_name_group.size() - 2);
                }
                else
                {
                    constant_type = ShaderElementType::ConstantBuffer;
                }

                mappings.emplace(export_type_group);
            }

            constants.emplace_back(
                ShaderConstantData{.name = constant_name, .constant_type = constant_type, .structure = -1});
        }
        return std::make_tuple(constants, mappings);
    }

    auto DXCCompiler::get_shader_technique(std::string const& shader_code) -> ShaderTechniqueData
    {
        std::regex const technique_pattern("technique\\s*\\{\\s*pass\\s*\\{((?:\\s+.+\\s+){1,})\\}\\s*\\}");

        std::smatch match;
        if (std::regex_search(shader_code, match, technique_pattern))
        {
            std::string const parameters_code = match[1].str();
            std::regex const parameter_pattern("(\\w+)\\s*=\\s*(.+);");

            auto shader_technique = ShaderTechniqueData{.vertex_stage = ShaderStageData{.buffer = -1},
                                                        .pixel_stage = ShaderStageData{.buffer = -1},
                                                        .compute_stage = ShaderStageData{.buffer = -1}};

            for (auto it = std::sregex_iterator(parameters_code.begin(), parameters_code.end(), parameter_pattern);
                 it != std::sregex_iterator(); ++it)
            {
                match = *it;

                std::string const parameter = match[1].str();
                std::string const value = match[2].str();

                if (parameter.compare("vertexShader") == 0)
                {
                    shader_technique.vertex_stage.entry_point = value.substr(0, value.size() - 2);
                }
                else if (parameter.compare("pixelShader") == 0)
                {
                    shader_technique.pixel_stage.entry_point = value.substr(0, value.size() - 2);
                }
                else if (parameter.compare("computeShader") == 0)
                {
                    shader_technique.compute_stage.entry_point = value.substr(0, value.size() - 2);
                }
                else if (parameter.compare("depthWrite") == 0)
                {
                    if (value.compare("true") == 0)
                    {
                        shader_technique.depth_write = true;
                    }
                    else if (value.compare("false") == 0)
                    {
                        shader_technique.depth_write = false;
                    }
                    else
                    {
                        // TO DO!
                    }
                }
                else if (parameter.compare("stencilWrite") == 0)
                {
                    if (value.compare("true") == 0)
                    {
                        shader_technique.stencil_write = true;
                    }
                    else if (value.compare("false") == 0)
                    {
                        shader_technique.stencil_write = false;
                    }
                    else
                    {
                        // TO DO!
                    }
                }
                else if (parameter.compare("cullSide") == 0)
                {
                    if (value.compare("\"back\"") == 0)
                    {
                        shader_technique.cull_side = ShaderCullSide::Back;
                    }
                    else if (value.compare("\"front\"") == 0)
                    {
                        shader_technique.cull_side = ShaderCullSide::Front;
                    }
                    else if (value.compare("\"none\"") == 0)
                    {
                        shader_technique.cull_side = ShaderCullSide::None;
                    }
                    else
                    {
                        // TO DO!
                    }
                }
            }
            return shader_technique;
        }
        else
        {
            // TO DO!
        }
    }

    auto DXCCompiler::get_shader_structures(std::string const& shader_code, std::set<std::string> const& mappings)
        -> std::vector<ShaderStructureData>
    {
        std::vector<ShaderStructureData> structures;

        std::regex const struct_pattern("struct\\s+(\\w+)\\s+\\{((?:\\s+.+\\s+){1,})\\};");

        for (auto it_struct = std::sregex_iterator(shader_code.begin(), shader_code.end(), struct_pattern);
             it_struct != std::sregex_iterator(); ++it_struct)
        {
            std::smatch match = *it_struct;

            std::string const struct_name_group = match[1].str();
            std::string const struct_data_group = match[2].str();

            if (input_assembler_names.find(struct_name_group) != input_assembler_names.end())
            {
                std::regex const struct_data_pattern(
                    "(bool|uint|float[2-4]x[2-4]|float[2-4]*)\\s+(\\w+)\\s*:\\s*(\\w+)\\s*;");

                std::vector<ShaderStructureElementData> elements;
                uint32_t size = 0;

                for (auto it_data =
                         std::sregex_iterator(struct_data_group.begin(), struct_data_group.end(), struct_data_pattern);
                     it_data != std::sregex_iterator(); ++it_data)
                {
                    match = *it_data;

                    std::string const element_type_group = match[1].str();
                    std::string const element_name_group = match[2].str();
                    std::string const semantic_group = match[3].str();

                    elements.emplace_back(
                        ShaderStructureElementData{.name = element_name_group,
                                                   .element_type = hlslconv::types[element_type_group],
                                                   .semantic = semantic_group});
                    size += hlslconv::sizes[element_type_group];
                }

                structures.emplace_back(
                    ShaderStructureData{.name = struct_name_group, .elements = std::move(elements), .size = size});
            }
            else
            {
                if (mappings.find(struct_name_group) != mappings.end())
                {
                    std::regex const struct_data_pattern(
                        "(Texture2D|bool|uint|float[2-4]x[2-4]|float[2-4]*)\\s+(\\w+);");

                    std::vector<ShaderStructureElementData> elements;
                    uint32_t size = 0;

                    for (auto it_data = std::sregex_iterator(struct_data_group.begin(), struct_data_group.end(),
                                                             struct_data_pattern);
                         it_data != std::sregex_iterator(); ++it_data)
                    {
                        std::smatch match = *it_data;

                        std::string const element_type_group = match[1].str();
                        std::string const element_name_group = match[2].str();

                        elements.emplace_back(ShaderStructureElementData{
                            .name = element_name_group, .element_type = hlslconv::types[element_type_group]});
                        size += hlslconv::sizes[element_type_group];
                    }

                    structures.emplace_back(
                        ShaderStructureData{.name = struct_name_group, .elements = std::move(elements), .size = size});
                }
            }
        }
        return structures;
    }

    auto DXCCompiler::generate_shader_code_v1(std::string const& shader_code,
                                              std::span<ShaderConstantData const> const constants) -> std::string
    {
        std::string new_shader_code = shader_code;

        std::regex const export_pattern("export\\s+(\\w+)\\s+(\\w+\\[\\]|\\w+);");

        uint32_t position = 0;
        std::smatch match;

        while (std::regex_search(new_shader_code, match, export_pattern))
        {
            position = match.position();
            auto begin_shader_code = new_shader_code.substr(0, match.position());
            auto end_shader_code = new_shader_code.substr(match.position() + match.length(), std::string::npos);
            new_shader_code = begin_shader_code + end_shader_code;
        }

        {
            auto begin_shader_code = new_shader_code.substr(0, position);
            auto end_shader_code = new_shader_code.substr(position, std::string::npos);

            new_shader_code = begin_shader_code;
            new_shader_code += "struct SHADER_DATA {\n";
            for (auto const& constant : constants)
            {
                switch (constant.constant_type)
                {
                    case ShaderElementType::Texture2D:
                    case ShaderElementType::ConstantBuffer:
                    case ShaderElementType::SamplerState:
                    case ShaderElementType::StorageBuffer: {
                        new_shader_code = new_shader_code + "    " + "uint" + " " + constant.name + ";\n";
                        break;
                    }
                }
            }
            new_shader_code += "};\n\n";
            new_shader_code += "ConstantBuffer<SHADER_DATA> shaderData : register(b0, space0);\n";
            new_shader_code += end_shader_code;
        }
        return new_shader_code;
    }

    auto DXCCompiler::generate_shader_code_v2(std::string const& shader_code, std::set<std::string> const& mappings)
        -> std::string
    {
        std::string new_shader_code = shader_code;

        std::regex const bindless_type_pattern("Texture2D |SamplerState ");

        // Idk how do it through std::regex_match replace only structs by name from mappings.
        // Bad variant but works.
        new_shader_code = std::regex_replace(new_shader_code, bindless_type_pattern, "uint ");

        std::regex const technique_pattern("technique\\s*\\{\\s*pass\\s*\\{((?:\\s+.+\\s+){1,})\\}\\s*\\}");

        new_shader_code = std::regex_replace(new_shader_code, technique_pattern, "");
        return new_shader_code;
    }
} // namespace ionengine::rhi::fx