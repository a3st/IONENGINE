// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "compiler.hpp"
#include "precompiled.h"

namespace ionengine::tools::shaderc
{
    DXC::DXC()
    {
        ::DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), compiler.put_void());
    }

    auto DXC::getTarget() const -> std::string_view
    {
        return "DXIL";
    }

    auto DXC::compile(std::string_view const shaderCode, std::filesystem::path const& outputPath,
                      std::string& errors) -> bool
    {
        return true;
    }

    /*
    auto DXCCompiler::compile(std::filesystem::path const& filePath, std::filesystem::path const& outPath) -> bool
    {
        std::string shaderCode;
        {
            std::basic_ifstream<char> stream(filePath);
            if (!stream.is_open())
            {
                std::cout << 1 << std::endl;
            }
            shaderCode = {std::istreambuf_iterator<char>(stream.rdbuf()), {}};
        }

        if (!mergeShaderCode(shaderCode))
        {
            return false;
        }

        std::vector<ShaderStructureData> structures;

        if (!buildShaderStructures(shaderCode, structures))
        {
            return false;
        }

        std::vector<ShaderConstantData> constants;

        if (!buildShaderConstants(shaderCode, constants, structures))
        {
            return false;
        }

        ShaderTechniqueData technique;

        if (!buildShaderTechnique(shaderCode, technique))
        {
            return false;
        }

        if (!buildBindlessData(shaderCode, constants, structures, technique))
        {
            return false;
        }

        winrt::com_ptr<IDxcCompiler3> compiler;
        ::DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), compiler.put_void());

        DxcBuffer buffer;
        buffer.Ptr = shaderCode.c_str();
        buffer.Size = shaderCode.size();
        buffer.Encoding = DXC_CP_UTF8;

        std::vector<std::vector<uint8_t>> buffers;

        for (auto& [stage_name, stage_info] : technique.stages)
        {
            std::vector<LPCWSTR> arguments;

            if (stage_name.compare("vertexShader") == 0)
            {
                arguments.emplace_back(L"-T vs_6_6");
            }
            else if (stage_name.compare("pixelShader") == 0)
            {
                arguments.emplace_back(L"-T ps_6_6");
            }
            else if (stage_name.compare("computeShader") == 0)
            {
                arguments.emplace_back(L"-T cs_6_6");
            }

            std::wstring entry_point_parameter;
            {
                std::string const key = "-E " + stage_info.entryPoint;

                size_t length = strlen(reinterpret_cast<const char*>(key.data())) + 1;
                size_t result = 0;

                entry_point_parameter.resize(length - 1);

                mbstowcs_s(&result, entry_point_parameter.data(), length, key.data(), length - 1);
            }
            arguments.emplace_back(entry_point_parameter.c_str());

            arguments.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL3);

            winrt::com_ptr<IDxcResult> result;
            compiler->Compile(&buffer, arguments.data(), static_cast<uint32_t>(arguments.size()), nullptr,
                              __uuidof(IDxcResult), result.put_void());

            winrt::com_ptr<IDxcBlobUtf8> errors;
            result->GetOutput(DXC_OUT_ERRORS, __uuidof(IDxcBlobUtf8), errors.put_void(), nullptr);

            if (errors && errors->GetStringLength() > 0)
            {
                std::cerr << std::string_view(reinterpret_cast<char*>(errors->GetBufferPointer()),
                                              errors->GetBufferSize())
                          << std::endl;
            }

            winrt::com_ptr<IDxcBlob> out;
            result->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), out.put_void(), nullptr);

            stage_info.buffer = buffers.size();

            std::vector<uint8_t> buffer(out->GetBufferSize());
            std::memcpy(buffer.data(), out->GetBufferPointer(), out->GetBufferSize());
            buffers.emplace_back(std::move(buffer));
        }

        ShaderEffect effect = {.target = ShaderTargetType::DXIL,
                               .technique = technique,
                               .constants = constants,
                               .structures = structures,
                               .buffers = buffers};
        return FXSL::save(effect, outPath);
    }



    auto DXCCompiler::buildShaderConstants(std::string& shader_code, std::vector<ShaderConstantData>& constants,
                                           std::span<ShaderStructureData const> const structures) -> bool
    {
        
    }

    auto DXCCompiler::buildShaderStructures(std::string& shader_code,
                                            std::vector<ShaderStructureData>& structures) -> bool
    {
        uint64_t offset = 0;

        while (offset != std::string::npos)
        {
            offset = shader_code.find("struct", offset);

            if (offset == std::string::npos)
            {
                break;
            }

            offset = shader_code.find(" ", offset);

            uint64_t const br_open = shader_code.find("{", offset) + 1;
            uint64_t br_close = 0;

            auto struct_name = std::string(shader_code.begin() + offset, shader_code.begin() + br_open - 1);

            struct_name.erase(struct_name.begin(), std::find_if(struct_name.begin(), struct_name.end(),
                                                                [](unsigned char ch) { return !std::isspace(ch); }));
            struct_name.erase(std::find_if(struct_name.rbegin(), struct_name.rend(),
                                           [](unsigned char ch) { return !std::isspace(ch); })
                                  .base(),
                              struct_name.end());

            offset = br_open + 1;

            if (inputAssemblerNames.find(struct_name) != inputAssemblerNames.end())
            {
                std::vector<ShaderStructureElementData> elements;
                uint32_t size = 0;

                do
                {
                    br_close = shader_code.find("};", offset);
                    uint64_t const quote = shader_code.find(";", offset);

                    auto member = std::string(shader_code.begin() + offset, shader_code.begin() + quote);

                    member.erase(member.begin(), std::find_if(member.begin(), member.end(),
                                                              [](unsigned char ch) { return !std::isspace(ch); }));

                    auto member_type = std::string(member.begin(), member.begin() + member.find(" ", 0));

                    auto member_name =
                        std::string(member.begin() + member.find(" ", 0), member.begin() + member.find(":", 0));
                    member_name.erase(member_name.begin(),
                                      std::find_if(member_name.begin(), member_name.end(),
                                                   [](unsigned char ch) { return !std::isspace(ch); }));
                    member_name.erase(std::find_if(member_name.rbegin(), member_name.rend(),
                                                   [](unsigned char ch) { return !std::isspace(ch); })
                                          .base(),
                                      member_name.end());

                    auto member_semantic = std::string(member.begin() + member.find(":", 0) + 1, member.end());
                    member_semantic.erase(member_semantic.begin(),
                                          std::find_if(member_semantic.begin(), member_semantic.end(),
                                                       [](unsigned char ch) { return !std::isspace(ch); }));
                    member_semantic.erase(std::find_if(member_semantic.rbegin(), member_semantic.rend(),
                                                       [](unsigned char ch) { return !std::isspace(ch); })
                                              .base(),
                                          member_semantic.end());

                    elements.emplace_back(ShaderStructureElementData{
                        .name = member_name, .elementType = hlslconv::types[member_type], .semantic = member_semantic});
                    size += hlslconv::sizes[member_type];

                    offset = quote + 1;

                } while (offset < br_close - 1);

                structures.emplace_back(
                    ShaderStructureData{.name = struct_name, .elements = std::move(elements), .size = size});
            }
            else
            {
                std::vector<ShaderStructureElementData> elements;
                uint32_t size = 0;

                do
                {
                    br_close = shader_code.find("};", offset);
                    uint64_t const quote = shader_code.find(";", offset);

                    auto member = std::string(shader_code.begin() + offset, shader_code.begin() + quote);

                    member.erase(member.begin(), std::find_if(member.begin(), member.end(),
                                                              [](unsigned char ch) { return !std::isspace(ch); }));

                    auto member_type = std::string(member.begin(), member.begin() + member.find(" ", 0));

                    auto member_name = std::string(member.begin() + member.find(" ", 0), member.end());
                    member_name.erase(member_name.begin(),
                                      std::find_if(member_name.begin(), member_name.end(),
                                                   [](unsigned char ch) { return !std::isspace(ch); }));
                    member_name.erase(std::find_if(member_name.rbegin(), member_name.rend(),
                                                   [](unsigned char ch) { return !std::isspace(ch); })
                                          .base(),
                                      member_name.end());

                    switch (hlslconv::types[member_type])
                    {
                        case ShaderElementType::Texture2D: {
                            uint64_t const begin_member_offset = shader_code.find(member_type, offset);
                            uint64_t const end_member_offset = shader_code.find(member_name, begin_member_offset);

                            auto begin_shader = shader_code.substr(0, begin_member_offset);
                            auto end_shader = shader_code.substr(end_member_offset, std::string::npos);

                            offset -= shader_code.size() - (begin_shader.size() + end_shader.size());
                            shader_code = begin_shader + "uint " + end_shader;
                            break;
                        }
                        case ShaderElementType::SamplerState: {
                            uint64_t const begin_member_offset = shader_code.find(member_type, offset);
                            uint64_t const end_member_offset = shader_code.find(member_name, begin_member_offset);

                            auto begin_shader = shader_code.substr(0, begin_member_offset);
                            auto end_shader = shader_code.substr(end_member_offset, std::string::npos);

                            offset -= shader_code.size() - (begin_shader.size() + end_shader.size());
                            shader_code = begin_shader + "uint " + end_shader;
                            break;
                        }
                    }

                    elements.emplace_back(
                        ShaderStructureElementData{.name = member_name, .elementType = hlslconv::types[member_type]});
                    size += hlslconv::sizes[member_type];

                    offset = quote + 1;

                } while (offset < br_close - 1);

                structures.emplace_back(
                    ShaderStructureData{.name = struct_name, .elements = std::move(elements), .size = size});
            }
        }
        return true;
    }

    auto DXCCompiler::buildBindlessData(std::string& shader_code,
                                                 std::span<ShaderConstantData const> const constants,
                                                 std::span<ShaderStructureData const> const structures,
                                                 ShaderTechniqueData const& technique) -> bool
    {
        for (auto const& [stage_name, stage_info] : technique.stages)
        {
            uint64_t offset = 0;
            offset = shader_code.find(stage_info.entryPoint, offset);

            uint64_t const br_open = shader_code.find("{", offset);
            uint64_t br_close = 0;

            for (auto const& constant : constants)
            {
                offset = shader_code.find(constant.name, br_open);

                if (offset == std::string::npos)
                {
                    continue;
                }

                do
                {
                    br_close = shader_code.find("}", offset);

                    uint64_t const name_offset = shader_code.find(constant.name, offset);

                    if (name_offset == std::string::npos)
                    {
                        break;
                    }

                    switch (constant.constantType)
                    {
                        case ShaderElementType::ConstantBuffer: {
                            auto member = std::string(shader_code.begin() + name_offset,
                                                      shader_code.begin() + name_offset + constant.name.size());

                            uint64_t const dot_offset = shader_code.find(".", name_offset);
                            uint64_t constant_member_end_offset = shader_code.find(" ", dot_offset);

                            if (constant_member_end_offset == std::string::npos)
                            {
                                constant_member_end_offset = shader_code.find(",", dot_offset);
                            }
                            if (constant_member_end_offset == std::string::npos)
                            {
                                // FAIL
                            }

                            auto constant_member = std::string(shader_code.begin() + dot_offset + 1,
                                                               shader_code.begin() + constant_member_end_offset - 1);

                            auto const& structure = structures[constant.structure];

                            auto it =
                                std::find_if(structure.elements.begin(), structure.elements.end(),
                                             [&](auto const& element) { return element.name == constant_member; });

                            if (it == structure.elements.end())
                            {
                            }

                            switch (it->elementType)
                            {
                                case ShaderElementType::Texture2D: {
                                    auto begin_shader = shader_code.substr(0, name_offset);
                                    auto end_shader =
                                        shader_code.substr(constant_member_end_offset - 1, std::string::npos);
                                    auto gen_shader =
                                        "Texture2D(ResourceDescriptorHeap[NonUniformResourceIndex(ConstantBuffer<" +
                                        structure.name + ">(ResourceDescriptorHeap[shaderData." + member + "])." +
                                        constant_member + ")])";

                                    offset = name_offset + 1 + gen_shader.size() -
                                             (shader_code.size() - (begin_shader.size() + end_shader.size()));

                                    shader_code = begin_shader + gen_shader + end_shader;
                                    break;
                                }
                                default: {
                                    auto begin_shader = shader_code.substr(0, name_offset);
                                    auto end_shader =
                                        shader_code.substr(constant_member_end_offset - 1, std::string::npos);
                                    auto gen_shader = "ConstantBuffer<" + structure.name +
                                                      ">(ResourceDescriptorHeap[shaderData." + member + "])." +
                                                      constant_member;

                                    offset = name_offset + 1 + gen_shader.size() -
                                             (shader_code.size() - (begin_shader.size() + end_shader.size()));

                                    shader_code = begin_shader + gen_shader + end_shader;
                                    break;
                                }
                            }
                            break;
                        }
                        default: {
                            offset = name_offset + 1;
                            break;
                        }
                    }

                } while (offset < br_close - 1);
            }
        }
        return true;
    }

    auto DXCCompiler::buildShaderTechnique(std::string& shader_code, ShaderTechniqueData& technique) -> bool
    {
        std::regex const technique_pattern("technique\\s*\\{\\s*pass\\s*\\{((?:\\s+.+\\s+){1,})\\}\\s*\\}");

        std::smatch match;
        if (std::regex_search(shader_code, match, technique_pattern))
        {
            std::string const parameters_code = match[1].str();
            std::regex const parameter_pattern("(\\w+)\\s*=\\s*(.+);");

            for (auto it = std::sregex_iterator(parameters_code.begin(), parameters_code.end(), parameter_pattern);
                 it != std::sregex_iterator(); ++it)
            {
                match = *it;

                std::string const parameter = match[1].str();
                std::string const value = match[2].str();

                if (parameter.compare("vertexShader") == 0)
                {
                    technique.stages.emplace("vertexShader",
                                             ShaderStageData{.entryPoint = value.substr(0, value.size() - 2)});
                }
                else if (parameter.compare("pixelShader") == 0)
                {
                    technique.stages.emplace("pixelShader",
                                             ShaderStageData{.entryPoint = value.substr(0, value.size() - 2)});
                }
                else if (parameter.compare("computeShader") == 0)
                {
                    technique.stages.emplace("computeShader",
                                             ShaderStageData{.entryPoint = value.substr(0, value.size() - 2)});
                }
                else if (parameter.compare("depthWrite") == 0)
                {
                    if (value.compare("true") == 0)
                    {
                        technique.depthWrite = true;
                    }
                    else if (value.compare("false") == 0)
                    {
                        technique.depthWrite = false;
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
                        technique.stencilWrite = true;
                    }
                    else if (value.compare("false") == 0)
                    {
                        technique.stencilWrite = false;
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
                        technique.cullSide = ShaderCullSide::Back;
                    }
                    else if (value.compare("\"front\"") == 0)
                    {
                        technique.cullSide = ShaderCullSide::Front;
                    }
                    else if (value.compare("\"none\"") == 0)
                    {
                        technique.cullSide = ShaderCullSide::None;
                    }
                    else
                    {
                        // TO DO!
                    }
                }
            }
        }
        else
        {
            // TO DO!
        }

        shader_code = std::regex_replace(shader_code, technique_pattern, "");
        return true;
    }*/
} // namespace ionengine::tools::shaderc