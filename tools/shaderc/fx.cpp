// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "fx.hpp"
#include "precompiled.h"

namespace ionengine::tools::shaderc
{
    namespace hlslconv
    {
        std::unordered_map<std::string, rhi::fx::ShaderElementType> types = {
            {"float4x4", rhi::fx::ShaderElementType::Float4x4},
            {"float3x3", rhi::fx::ShaderElementType::Float3x3},
            {"float2x2", rhi::fx::ShaderElementType::Float2x2},
            {"float4", rhi::fx::ShaderElementType::Float4},
            {"float3", rhi::fx::ShaderElementType::Float3},
            {"float2", rhi::fx::ShaderElementType::Float2},
            {"float", rhi::fx::ShaderElementType::Float},
            {"bool", rhi::fx::ShaderElementType::Bool},
            {"uint", rhi::fx::ShaderElementType::Uint},
            {"SamplerState", rhi::fx::ShaderElementType::SamplerState},
            {"Texture2D", rhi::fx::ShaderElementType::Texture2D}};

        std::unordered_map<std::string, size_t> sizes = {
            {"float4x4", 64}, {"float3x3", 36}, {"float2x2", 16}, {"float4", 16},      {"float3", 12},  {"float2", 8},
            {"float", 4},     {"bool", 4},      {"uint", 4},      {"SamplerState", 4}, {"Texture2D", 4}};
    } // namespace hlslconv

    FXCompiler::FXCompiler()
    {
        inputAssemblerNames.emplace("VS_INPUT");
        inputAssemblerNames.emplace("VS_OUTPUT");
        inputAssemblerNames.emplace("PS_OUTPUT");
    }

    auto FXCompiler::addIncludePath(std::filesystem::path const& includePath) -> void
    {
        includePaths.emplace_back(includePath);
    }

    auto FXCompiler::compile(ShaderCompiler& compiler, std::filesystem::path const& inputPath,
                             std::filesystem::path const& outputPath, std::string& errors) -> bool
    {

        std::string shaderCode;
        {
            std::basic_ifstream<char> stream(inputPath);
            if (!stream.is_open())
            {
                errors = "An error occurred while opening the input file";
                return false;
            }
            shaderCode = {std::istreambuf_iterator<char>(stream.rdbuf()), {}};
        }

        if (!mergeShaderCode(shaderCode))
        {
            errors = "An error occurred while merging include files";
            return false;
        }

        std::vector<rhi::fx::ShaderStructureData> structures;

        std::vector<rhi::fx::ShaderConstantData> constants;

        if (!generateShaderConstants(shaderCode, constants, structures))
        {
            return false;
        }

        std::cout << shaderCode << std::endl;

        return true;
    }

    auto FXCompiler::mergeShaderCode(std::string& shaderCode) -> bool
    {
        uint64_t offset = 0;
        while (offset != std::string::npos)
        {
            offset = shaderCode.find("#include", offset);
            if (offset == std::string::npos)
            {
                break;
            }

            uint64_t const quoteOpenOffset = shaderCode.find("\"", offset);
            if (quoteOpenOffset == std::string::npos)
            {
                return false;
            }

            uint64_t const qouteCloseOffset = shaderCode.find("\"", quoteOpenOffset + 1);
            if (qouteCloseOffset == std::string::npos)
            {
                return false;
            }

            std::string const includeFile(shaderCode.begin() + quoteOpenOffset + 1,
                                          shaderCode.begin() + qouteCloseOffset);

            for (auto const& includePath : includePaths)
            {
                if (std::filesystem::exists(includePath / includeFile))
                {
                    std::string includeShaderCode;
                    {
                        std::basic_ifstream<char> stream(includePath / includeFile);
                        if (!stream.is_open())
                        {
                            return false;
                        }
                        includeShaderCode = {std::istreambuf_iterator<char>(stream.rdbuf()), {}};
                    }

                    auto beginShaderCode = shaderCode.substr(0, offset);
                    auto endShaderCode = shaderCode.substr(qouteCloseOffset, std::string::npos);
                    shaderCode = beginShaderCode + includeShaderCode + endShaderCode;
                    break;
                }
            }

            offset = qouteCloseOffset + 1;
        }
        return true;
    }

    auto FXCompiler::generateShaderConstants(std::string& shaderCode,
                                             std::vector<rhi::fx::ShaderConstantData>& constants,
                                             std::span<rhi::fx::ShaderStructureData const> const structures) -> bool
    {
        uint64_t offset = 0;
        while (offset != std::string::npos)
        {
            offset = shaderCode.find("[[fx::shader_constant]]", offset);
            if (offset == std::string::npos)
            {
                break;
            }

            offset = shaderCode.find(" ", offset);

            uint64_t const constantTypeOffset = shaderCode.find(" ", offset);
            if (constantTypeOffset == std::string::npos)
            {
                return false;
            }

            uint64_t const constantNameOffset = shaderCode.find(" ", constantTypeOffset);
            if (constantNameOffset == std::string::npos)
            {
                return false;
            }

            uint64_t const constantDotOffset = shaderCode.find(";", constantNameOffset);
            if (constantDotOffset == std::string::npos)
            {
                return false;
            }

            std::cout << std::string(shaderCode.begin() + offset, shaderCode.begin() + constantTypeOffset) << std::endl;
        }
        /*
        std::regex const export_pattern("export\\s+(\\w+)\\s+(\\w+\\[\\]|\\w+);\\s*");

        std::smatch match;
        uint32_t position = 0;

        while (std::regex_search(shader_code, match, export_pattern))
        {
            position = match.position();

            std::string const export_type_group = match[1].str();
            std::string const struct_name_group = match[2].str();

            ShaderElementType constant_type;
            std::string constant_name = struct_name_group;
            int32_t structure = -1;

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

                auto it = std::find_if(structures.begin(), structures.end(),
                                       [&](auto const& element) { return element.name == export_type_group; });

                if (it != structures.end())
                {
                    structure = std::distance(structures.begin(), it);
                }
            }

            constants.emplace_back(
                ShaderConstantData{.name = constant_name, .constantType = constant_type, .structure = structure});

            auto begin_shader_code = shader_code.substr(0, match.position());
            auto end_shader_code = shader_code.substr(match.position() + match.length(), std::string::npos);
            shader_code = begin_shader_code + end_shader_code;
        }

        auto begin_shader_code = shader_code.substr(0, position);
        auto end_shader_code = shader_code.substr(position, std::string::npos);

        shader_code = begin_shader_code;
        shader_code += "struct SHADER_DATA {\n";
        for (auto const& constant : constants)
        {
            switch (constant.constantType)
            {
                case ShaderElementType::Texture2D:
                case ShaderElementType::ConstantBuffer:
                case ShaderElementType::SamplerState:
                case ShaderElementType::StorageBuffer: {
                    shader_code = shader_code + "    " + "uint" + " " + constant.name + ";\n";
                    break;
                }
            }
        }
        shader_code += "};\n\n";
        shader_code += "ConstantBuffer<SHADER_DATA> shaderData : register(b0, space0);\n\n";
        shader_code += end_shader_code;
        return true;
        */

        return true;
    }
} // namespace ionengine::tools::shaderc