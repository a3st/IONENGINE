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
            {"float2x2", ShaderElementType::Float4x4},
            {"float4", ShaderElementType::Float4},
            {"float3", ShaderElementType::Float3},
            {"float2", ShaderElementType::Float2},
            {"float", ShaderElementType::Float},
            {"bool", ShaderElementType::Bool},
            {"uint", ShaderElementType::Uint},
            {"SamplerState", ShaderElementType::SamplerState},
            {"Texture2D<float4>", ShaderElementType::Texture2D},
            {"Texture2D<float3>", ShaderElementType::Texture2D}};
    }

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
        std::vector<ShaderConstantData> constants = get_shader_constants(shader_code);

        std::vector<ShaderStructureData> structures;

        /*std::regex const struct_pattern("struct\\s+(\\w+)\\s+\\{((?:\\s+.+\\s+){1,})\\};");

        for (auto it = std::sregex_iterator(shader_code.begin(), shader_code.end(), struct_pattern);
             it != std::sregex_iterator(); ++it)
        {
            std::smatch match = *it;

            std::string const struct_name_group = match[1].str();
            std::string const struct_data_group = match[2].str();

            if (input_assembler_names.find(struct_name_group) != input_assembler_names.end())
            {
                std::regex const struct_data_pattern(
                    "(bool|uint|float[2-4]x[2-4]|float[2-4]*)\\s+(\\w+)\\s*:\\s*(\\w+)\\s*;");

                for (auto it2 =
                         std::sregex_iterator(struct_data_group.begin(), struct_data_group.end(), struct_data_pattern);
                     it2 != std::sregex_iterator(); ++it2)
                {
                    std::smatch match2 = *it2;

                    auto element_data = ShaderStructureElementData{.name = match2[2].str(),
                                                                   .element_type = hlslconv::types[match2[1].str()],
                                                                   .semantic = match2[3].str()};

                    //std::cout << " " << match2[1].str() << std::endl;
                }
            }
            else
            {
                std::regex const struct_data_pattern(
                    "(Texture2D<float[3-4]>|bool|uint|float[2-4]x[2-4]|float[2-4]*)\\s+(\\w+);");

                if (shader_resource_names.find(struct_name_group) != shader_resource_names.end())
                {
                    for (auto it2 = std::sregex_iterator(struct_data_group.begin(), struct_data_group.end(),
                                                         struct_data_pattern);
                         it2 != std::sregex_iterator(); ++it2)
                    {
                        std::smatch match2 = *it2;

                        auto element_data = ShaderStructureElementData{
                            .name = match2[2].str(), .element_type = hlslconv::types[match2[1].str()]};

                        // std::cout << " " << match2[1].str() << std::endl;
                    }
                }
            }
        }*/
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
                    auto end_shader_code = shader_code.substr(match.length(), shader_code.size());
                    shader_code = begin_shader_code + include_shader_code + end_shader_code;
                }
            }
        }
        return shader_code;
    }

    auto DXCCompiler::get_shader_constants(std::string const& shader_code) -> std::vector<ShaderConstantData>
    {
        std::vector<ShaderConstantData> constants;

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
            }

            constants.emplace_back(
                ShaderConstantData{.name = constant_name, .constant_type = constant_type, .structure = -1});
        }
        return constants;
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

    /*auto DXCCompiler::parse_shader_resources(std::string const& shader_code) -> std::set<std::string>
    {
        std::set<std::string> shader_resource_names;
        std::regex const struct_pattern("struct\\s+(\\w+)\\s+\\{((?:\\s+.+\\s+){1,})\\};");

        for (auto it_struct = std::sregex_iterator(shader_code.begin(), shader_code.end(), struct_pattern);
             it_struct != std::sregex_iterator(); ++it_struct)
        {
            std::smatch match = *it_struct;

            std::string const struct_name_group = match[1].str();
            std::string const struct_data_group = match[2].str();

            if (struct_name_group.compare("SHADER_DATA") == 0)
            {
                std::regex const struct_data_pattern("(\\w+)\\s+(\\w+);");

                for (auto it_data =
                         std::sregex_iterator(struct_data_group.begin(), struct_data_group.end(), struct_data_pattern);
                     it_data != std::sregex_iterator(); ++it_data)
                {
                    match = *it_data;

                    std::string const element_type_group = match[1].str();

                    if (hlslconv::types.find(element_type_group) == hlslconv::types.end())
                    {
                        shader_resource_names.emplace(element_type_group);
                    }
                }
                break;
            }
        }
        return shader_resource_names;
    }*/
} // namespace ionengine::rhi::fx