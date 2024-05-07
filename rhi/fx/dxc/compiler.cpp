// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "compiler.hpp"
#include "precompiled.h"

namespace ionengine::rhi::fx
{
    namespace hlslconv
    {
        std::unordered_map<std::string, ShaderElementType> types = {
            {"float4x4", ShaderElementType::Float4x4},  {"float3x3", ShaderElementType::Float3x3},
            {"float2x2", ShaderElementType::Float2x2},  {"float4", ShaderElementType::Float4},
            {"float3", ShaderElementType::Float3},      {"float2", ShaderElementType::Float2},
            {"float", ShaderElementType::Float},        {"bool", ShaderElementType::Bool},
            {"uint", ShaderElementType::Uint},          {"SamplerState", ShaderElementType::SamplerState},
            {"Texture2D", ShaderElementType::Texture2D}};

        std::unordered_map<std::string, size_t> sizes = {
            {"float4x4", 64}, {"float3x3", 36}, {"float2x2", 16}, {"float4", 16},      {"float3", 12},  {"float2", 8},
            {"float", 4},     {"bool", 4},      {"uint", 4},      {"SamplerState", 4}, {"Texture2D", 4}};
    } // namespace hlslconv

    template <typename Type>
    auto to_string(Type const type) -> std::string;

    template <>
    auto to_string(ShaderElementType const element_type) -> std::string
    {
        case ShaderElementType::Bool: {
            element_type = "BOOL";
            break;
        }
    }

    auto from_string(std::string_view const element_type) -> ShaderElementType;

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

    auto DXCCompiler::compile(std::filesystem::path const& file_path) -> std::optional<std::vector<uint8_t>>
    {
        std::string shader_code;
        {
            std::basic_ifstream<char> stream(file_path);
            if (!stream.is_open())
            {
                std::cout << 1 << std::endl;
            }
            shader_code = {std::istreambuf_iterator<char>(stream.rdbuf()), {}};
        }

        if (!merge_shader_code(shader_code))
        {
            return std::nullopt;
        }

        std::vector<ShaderStructureData> structures;

        if (!convert_shader_structures(shader_code, structures))
        {
            return std::nullopt;
        }

        std::vector<ShaderConstantData> constants;

        if (!convert_shader_constants(shader_code, constants, structures))
        {
            return std::nullopt;
        }

        ShaderTechniqueData technique;

        if (!convert_shader_technique(shader_code, technique))
        {
            return std::nullopt;
        }

        if (!convert_bindless_constants(shader_code, constants, structures, technique))
        {
            return std::nullopt;
        }

        std::cout << shader_code << std::endl;

        winrt::com_ptr<IDxcCompiler3> compiler;
        ::DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), compiler.put_void());

        DxcBuffer buffer;
        buffer.Ptr = shader_code.c_str();
        buffer.Size = shader_code.size();
        buffer.Encoding = DXC_CP_UTF8;

        std::vector<winrt::com_ptr<IDxcBlob>> buffers;

        std::stringstream json(std::ios::out);

        json << "{\"technique\":{";

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
                std::string const key = "-E " + stage_info.entry_point;

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

            winrt::com_ptr<IDxcBlob> shader;
            result->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), shader.put_void(), nullptr);

            stage_info.buffer = buffers.size();
            buffers.emplace_back(shader);

            json << "\"" + stage_name + "\":{\"buffer\":" + std::to_string(stage_info.buffer) + ",\"entryPoint\":\"" +
                        stage_info.entry_point + "\"},";
        }

        std::string cull_side;
        switch (technique.cull_side)
        {
            case ShaderCullSide::None: {
                cull_side = "none";
                break;
            }
            case ShaderCullSide::Back: {
                cull_side = "back";
                break;
            }
            case ShaderCullSide::Front: {
                cull_side = "front";
                break;
            }
        }

        json << "\"depthWrite\":" << std::boolalpha << technique.depth_write << ",\"stencilWrite\":" << std::boolalpha
             << technique.stencil_write << ",\"cullSide\":"
             << "\"" + cull_side + "\"},\"constants\":[";

        for (auto const& constant : constants)
        {
            std::string element_type;
            switch (constant.constant_type)
            {
                case ShaderElementType::Bool: {
                    element_type = "BOOL";
                    break;
                }
            }

            json << "{\"name\":\"" + constant.name + "\",\"type\":\"";
        }

        std::cout << json.str() << std::endl;

        std::basic_stringstream<uint8_t> stream(std::ios::binary | std::ios::out);

        Header header = {.magic = Magic, .length = 0, .target = static_cast<uint32_t>(ShaderTargetType::DXIL)};

        stream.write(reinterpret_cast<uint8_t*>(&header), sizeof(Header));

        ChunkHeader json_chunk = {
            .chunk_type = static_cast<uint32_t>(ChunkType::Json),
        };

        return std::nullopt;
    }

    auto DXCCompiler::merge_shader_code(std::string& shader_code) -> bool
    {
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
                            return false;
                        }
                        include_shader_code = {std::istreambuf_iterator<char>(stream.rdbuf()), {}};
                    }

                    auto begin_shader_code = shader_code.substr(0, match.position());
                    auto end_shader_code = shader_code.substr(match.position() + match.length(), std::string::npos);
                    shader_code = begin_shader_code + include_shader_code + end_shader_code;
                }
            }
        }
        return true;
    }

    auto DXCCompiler::convert_shader_constants(std::string& shader_code, std::vector<ShaderConstantData>& constants,
                                               std::span<ShaderStructureData const> const structures) -> bool
    {
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
                ShaderConstantData{.name = constant_name, .constant_type = constant_type, .structure = structure});

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
            switch (constant.constant_type)
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
    }

    auto DXCCompiler::convert_shader_structures(std::string& shader_code, std::vector<ShaderStructureData>& structures)
        -> bool
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

            if (input_assembler_names.find(struct_name) != input_assembler_names.end())
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

                    elements.emplace_back(ShaderStructureElementData{.name = member_name,
                                                                     .element_type = hlslconv::types[member_type],
                                                                     .semantic = member_semantic});
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
                        ShaderStructureElementData{.name = member_name, .element_type = hlslconv::types[member_type]});
                    size += hlslconv::sizes[member_type];

                    offset = quote + 1;

                } while (offset < br_close - 1);

                structures.emplace_back(
                    ShaderStructureData{.name = struct_name, .elements = std::move(elements), .size = size});
            }
        }
        return true;
    }

    auto DXCCompiler::convert_bindless_constants(std::string& shader_code,
                                                 std::span<ShaderConstantData const> const constants,
                                                 std::span<ShaderStructureData const> const structures,
                                                 ShaderTechniqueData const& technique) -> bool
    {
        for (auto const& [stage_name, stage_info] : technique.stages)
        {
            uint64_t offset = 0;
            offset = shader_code.find(stage_info.entry_point, offset);

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

                    switch (constant.constant_type)
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

                            switch (it->element_type)
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

    auto DXCCompiler::convert_shader_technique(std::string& shader_code, ShaderTechniqueData& technique) -> bool
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
                                             ShaderStageData{.entry_point = value.substr(0, value.size() - 2)});
                }
                else if (parameter.compare("pixelShader") == 0)
                {
                    technique.stages.emplace("pixelShader",
                                             ShaderStageData{.entry_point = value.substr(0, value.size() - 2)});
                }
                else if (parameter.compare("computeShader") == 0)
                {
                    technique.stages.emplace("computeShader",
                                             ShaderStageData{.entry_point = value.substr(0, value.size() - 2)});
                }
                else if (parameter.compare("depthWrite") == 0)
                {
                    if (value.compare("true") == 0)
                    {
                        technique.depth_write = true;
                    }
                    else if (value.compare("false") == 0)
                    {
                        technique.depth_write = false;
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
                        technique.stencil_write = true;
                    }
                    else if (value.compare("false") == 0)
                    {
                        technique.stencil_write = false;
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
                        technique.cull_side = ShaderCullSide::Back;
                    }
                    else if (value.compare("\"front\"") == 0)
                    {
                        technique.cull_side = ShaderCullSide::Front;
                    }
                    else if (value.compare("\"none\"") == 0)
                    {
                        technique.cull_side = ShaderCullSide::None;
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
    }
} // namespace ionengine::rhi::fx