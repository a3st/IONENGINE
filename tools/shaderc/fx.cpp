// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "fx.hpp"
#include "core/error.hpp"
#include "core/serialize.hpp"
#include "core/string.hpp"
#include "precompiled.h"

namespace ionengine::tools::shaderc
{
    namespace hlslconv
    {
        std::unordered_map<std::string, rhi::fx::ShaderElementType> types{
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

        std::unordered_map<std::string, size_t> sizes{
            {"float4x4", 64}, {"float3x3", 36}, {"float2x2", 16}, {"float4", 16},      {"float3", 12},  {"float2", 8},
            {"float", 4},     {"bool", 4},      {"uint", 4},      {"SamplerState", 4}, {"Texture2D", 4}};
    } // namespace hlslconv

    auto throwIfFailed(HRESULT const hr) -> void
    {
        if (FAILED(hr))
        {
            throw core::runtime_error(std::format("The program closed with an error {:04x}", hr));
        }
    }

    FXCompiler::FXCompiler()
    {
        inputAssemblerNames.emplace("VS_INPUT");
        inputAssemblerNames.emplace("VS_OUTPUT");
        inputAssemblerNames.emplace("PS_OUTPUT");

        throwIfFailed(::DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), compiler.put_void()));
    }

    auto FXCompiler::addIncludePath(std::filesystem::path const& includePath) -> void
    {
        includePaths.emplace_back(includePath);
    }

    auto FXCompiler::compile(std::filesystem::path const& inputPath, std::filesystem::path const& outputPath,
                             std::string& errors) -> bool
    {
        std::string shaderCode;
        {
            std::basic_ifstream<char> fileStream(inputPath);
            if (!fileStream.is_open())
            {
                errors = "An error occurred while opening the input file";
                return false;
            }
            shaderCode = {std::istreambuf_iterator<char>(fileStream.rdbuf()), {}};
        }

        if (!mergeShaderCode(shaderCode))
        {
            errors = "An error occurred while merging include files";
            return false;
        }

        std::vector<rhi::fx::ShaderStructureData> structures;
        if (!generateShaderStructures(shaderCode, structures))
        {
            errors = "An error occurred while generation structures";
            return false;
        }

        std::vector<rhi::fx::ShaderConstantData> constants;
        if (!generateShaderConstants(shaderCode, constants, structures))
        {
            errors = "An error occurred while generation constants";
            return false;
        }

        rhi::fx::ShaderTechniqueData technique;
        if (!generateShaderTechnique(shaderCode, technique))
        {
            errors = "An error occurred while generation technique";
            return false;
        }

        if (!generateShaderBindlessData(shaderCode, constants, structures, technique))
        {
            errors = "An error occurred while generation bindless data";
            return false;
        }

        std::vector<std::vector<uint8_t>> buffers;
        for (auto& [stageType, stageInfo] : technique.stages)
        {
            auto result = compileShaderStage(shaderCode, stageInfo.entryPoint, stageType, errors);
            if (result.has_value())
            {
                stageInfo.buffer = buffers.size();
                buffers.emplace_back(result.value());
            }
            else
            {
                return false;
            }
        }

        rhi::fx::ShaderEffectFile effectFile = {.magic = rhi::fx::Magic,
                                                .target = rhi::fx::ShaderTargetType::DXIL,
                                                .effectData = {.technique = std::move(technique),
                                                               .constants = std::move(constants),
                                                               .structures = std::move(structures)},
                                                .buffers = std::move(buffers)};
        return core::save_to_file<rhi::fx::ShaderEffectFile, core::serialize_oarchive>(effectFile, outputPath);
    }

    auto FXCompiler::compileShaderStage(std::string_view const shaderCode, std::string_view const entryPoint,
                                        rhi::fx::ShaderStageType const stageType,
                                        std::string& errors) -> std::optional<std::vector<uint8_t>>
    {
        std::vector<LPCWSTR> arguments;

        switch (stageType)
        {
            case rhi::fx::ShaderStageType::Vertex: {
                arguments.emplace_back(L"-T vs_6_6");
                break;
            }
            case rhi::fx::ShaderStageType::Pixel: {
                arguments.emplace_back(L"-T ps_6_6");
                break;
            }
            case rhi::fx::ShaderStageType::Compute: {
                arguments.emplace_back(L"-T cs_6_6");
                break;
            }
        }

        std::wstring wArgument;
        {
            std::string const argument = "-E " + std::string(entryPoint);

            size_t length = strlen(reinterpret_cast<const char*>(argument.data())) + 1;
            size_t result = 0;

            wArgument.resize(length - 1);
            mbstowcs_s(&result, wArgument.data(), length, argument.data(), length - 1);
        }
        arguments.emplace_back(wArgument.c_str());

        arguments.emplace_back(DXC_ARG_OPTIMIZATION_LEVEL3);

        DxcBuffer buffer = {.Ptr = shaderCode.data(), .Size = shaderCode.size(), .Encoding = DXC_CP_UTF8};

        winrt::com_ptr<IDxcResult> result;
        compiler->Compile(&buffer, arguments.data(), static_cast<uint32_t>(arguments.size()), nullptr,
                          __uuidof(IDxcResult), result.put_void());

        winrt::com_ptr<IDxcBlobUtf8> errorsBlob;
        result->GetOutput(DXC_OUT_ERRORS, __uuidof(IDxcBlobUtf8), errorsBlob.put_void(), nullptr);

        if (errorsBlob && errorsBlob->GetStringLength() > 0)
        {
            errors = "\n" +
                     std::string(reinterpret_cast<char*>(errorsBlob->GetBufferPointer()), errorsBlob->GetBufferSize());
            return std::nullopt;
        }

        winrt::com_ptr<IDxcBlob> outBlob;
        result->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), outBlob.put_void(), nullptr);

        std::vector<uint8_t> outBuffer(outBlob->GetBufferSize());
        std::memcpy(outBuffer.data(), outBlob->GetBufferPointer(), outBuffer.size());
        return outBuffer;
    }

    auto FXCompiler::mergeShaderCode(std::string& shaderCode) -> bool
    {
        uint64_t offset = 0;

        while (true)
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

            uint64_t nextOffset = qouteCloseOffset + 1;

            for (auto const& includePath : includePaths)
            {
                if (std::filesystem::exists(includePath / includeFile))
                {
                    std::string includeShaderCode;
                    {
                        std::basic_ifstream<char> fileStream(includePath / includeFile);
                        if (!fileStream.is_open())
                        {
                            return false;
                        }
                        includeShaderCode = {std::istreambuf_iterator<char>(fileStream.rdbuf()), {}};
                    }

                    auto beginShaderCode = shaderCode.substr(0, offset);
                    auto endShaderCode = shaderCode.substr(qouteCloseOffset + 1, std::string::npos);
                    shaderCode = beginShaderCode + includeShaderCode + endShaderCode;
                    nextOffset = offset + 1 + includeShaderCode.size();
                    break;
                }
            }

            offset = nextOffset;
        }
        return true;
    }

    auto FXCompiler::generateShaderStructures(std::string& shaderCode,
                                              std::vector<rhi::fx::ShaderStructureData>& structures) -> bool
    {
        uint64_t offset = 0;

        while (true)
        {
            offset = shaderCode.find("struct", offset);
            if (offset == std::string::npos)
            {
                break;
            }

            offset = shaderCode.find(" ", offset);

            uint64_t const brOpenOffset = shaderCode.find("{", offset) + 1;
            uint64_t brCloseOffset = 0;

            std::string structureName(shaderCode.begin() + offset, shaderCode.begin() + brOpenOffset - 1);
            core::trim(structureName, core::trim_mode::both);

            offset = brOpenOffset + 1;

            std::vector<rhi::fx::ShaderStructureElementData> elements;
            uint32_t size = 0;

            do
            {
                brCloseOffset = shaderCode.find("};", offset);
                uint64_t const dotOffset = shaderCode.find(";", offset);

                if (brCloseOffset < dotOffset)
                {
                    break;
                }

                std::string member(shaderCode.begin() + offset, shaderCode.begin() + dotOffset);
                core::trim(member, core::trim_mode::both);

                std::string const memberType(member.begin(), member.begin() + member.find(" ", 0));

                uint64_t const semanticOffset = member.find(":", 0);

                rhi::fx::ShaderStructureElementData elementData;

                if (semanticOffset != std::string::npos)
                {
                    std::string memberSemantic(member.begin() + semanticOffset + 1, member.end());
                    core::trim(memberSemantic, core::trim_mode::both);

                    std::string memberName(member.begin() + member.find(" ", 0), member.begin() + semanticOffset);
                    core::trim(memberName, core::trim_mode::both);

                    elementData = {.elementName = memberName,
                                   .elementType = hlslconv::types[memberType],
                                   .semantic = memberSemantic};
                }
                else
                {
                    std::string memberName(member.begin() + member.find(" ", 0), member.end());
                    core::trim(memberName, core::trim_mode::both);

                    switch (hlslconv::types[memberType])
                    {
                        case rhi::fx::ShaderElementType::SamplerState:
                        case rhi::fx::ShaderElementType::Texture2D: {
                            uint64_t const beginMemberOffset = shaderCode.find(memberType, offset);
                            uint64_t const endMemberOffset = shaderCode.find(memberName, beginMemberOffset);

                            auto beginShader = shaderCode.substr(0, beginMemberOffset);
                            auto endShader = shaderCode.substr(endMemberOffset, std::string::npos);

                            offset -= shaderCode.size() - (beginShader.size() + endShader.size());
                            shaderCode = beginShader + "uint " + endShader;
                            break;
                        }
                        case rhi::fx::ShaderElementType::ConstantBuffer:
                        case rhi::fx::ShaderElementType::StorageBuffer: {
                            return false;
                        }
                    }

                    elementData = {.elementName = memberName, .elementType = hlslconv::types[memberType]};
                }

                elements.emplace_back(std::move(elementData));
                size += hlslconv::sizes[memberType];

                offset = dotOffset + 1;

            } while (offset < brCloseOffset - 1);

            rhi::fx::ShaderStructureData structureData{
                .structureName = structureName, .elements = std::move(elements), .size = size};
            structures.emplace_back(std::move(structureData));
        }
        return true;
    }

    auto FXCompiler::generateShaderConstants(std::string& shaderCode,
                                             std::vector<rhi::fx::ShaderConstantData>& constants,
                                             std::span<rhi::fx::ShaderStructureData const> const structures) -> bool
    {
        uint64_t offset = 0;
        uint64_t lastConstantOffset = 0;

        do
        {
            offset = shaderCode.find("[[fx::shader_constant]]", offset);
            if (offset == std::string::npos)
            {
                break;
            }

            lastConstantOffset = offset;

            uint64_t const constantTypeOffset = shaderCode.find(" ", offset);
            if (constantTypeOffset == std::string::npos)
            {
                return false;
            }

            uint64_t const constantNameOffset = shaderCode.find(" ", constantTypeOffset + 1);
            if (constantNameOffset == std::string::npos)
            {
                return false;
            }

            uint64_t const constantDotOffset = shaderCode.find(";", constantNameOffset);
            if (constantDotOffset == std::string::npos)
            {
                return false;
            }

            std::string const constantType(shaderCode.begin() + constantTypeOffset + 1,
                                           shaderCode.begin() + constantNameOffset);
            std::string const constantName(shaderCode.begin() + constantNameOffset + 1,
                                           shaderCode.begin() + constantDotOffset);

            rhi::fx::ShaderConstantData constantData;

            if (hlslconv::types.find(constantType) != hlslconv::types.end())
            {
                constantData = {
                    .constantName = constantName, .constantType = hlslconv::types[constantType], .structure = -1};
            }
            else
            {
                if (constantName.ends_with("[]"))
                {
                    constantData = {.constantName = constantName.substr(0, constantName.size() - 2),
                                    .constantType = rhi::fx::ShaderElementType::StorageBuffer};
                }
                else
                {
                    constantData = {.constantName = constantName,
                                    .constantType = rhi::fx::ShaderElementType::ConstantBuffer};
                }

                auto it = std::find_if(structures.begin(), structures.end(),
                                       [&](auto const& element) { return element.structureName == constantType; });

                if (it != structures.end())
                {
                    constantData.structure = static_cast<uint32_t>(std::distance(structures.begin(), it));
                }
            }

            constants.emplace_back(std::move(constantData));

            auto beginShaderCode = shaderCode.substr(0, offset);
            auto endShaderCode = shaderCode.substr(constantDotOffset + 2, std::string::npos);
            shaderCode = beginShaderCode + endShaderCode;

        } while (offset != std::string::npos);

        auto beginShaderCode = shaderCode.substr(0, lastConstantOffset);
        auto endShaderCode = shaderCode.substr(lastConstantOffset, std::string::npos);

        std::stringstream generatedShaderCode;
        generatedShaderCode << "struct SHADER_DATA {\n";
        for (auto const& constant : constants)
        {
            switch (constant.constantType)
            {
                case rhi::fx::ShaderElementType::Texture2D:
                case rhi::fx::ShaderElementType::ConstantBuffer:
                case rhi::fx::ShaderElementType::SamplerState:
                case rhi::fx::ShaderElementType::StorageBuffer: {
                    generatedShaderCode << " uint " << constant.constantName << ";\n";
                    break;
                }
            }
        }
        generatedShaderCode << "};\n\nConstantBuffer<SHADER_DATA> shaderData : register(b0, space0);\n";

        shaderCode = beginShaderCode + generatedShaderCode.str() + endShaderCode;
        return true;
    }

    auto FXCompiler::generateShaderTechnique(std::string& shaderCode, rhi::fx::ShaderTechniqueData& technique) -> bool
    {
        uint64_t offset = shaderCode.find("technique");
        if (offset == std::string::npos)
        {
            return false;
        }

        uint64_t const techniqueOffset = offset;

        offset = shaderCode.find("{", offset);
        if (offset == std::string::npos)
        {
            return false;
        }

        offset = shaderCode.find("pass", offset);
        if (offset == std::string::npos)
        {
            return false;
        }

        uint64_t const parametersBeginOffset = shaderCode.find("{", offset);
        if (parametersBeginOffset == std::string::npos)
        {
            return false;
        }

        uint64_t const parametersEndOffset = shaderCode.find("}", parametersBeginOffset);
        if (parametersEndOffset == std::string::npos)
        {
            return false;
        }

        offset = shaderCode.find("}", parametersEndOffset + 1);
        if (offset == std::string::npos)
        {
            return false;
        }

        std::regex const parameterRegex("(\\w+)\\s*=\\s*(.+);");
        for (auto it = std::sregex_iterator(shaderCode.begin() + parametersBeginOffset,
                                            shaderCode.begin() + parametersEndOffset, parameterRegex);
             it != std::sregex_iterator(); ++it)
        {
            std::smatch match = *it;

            std::string const parameter = match[1].str();
            std::string const value = match[2].str();

            if (parameter.compare("vertexShader") == 0)
            {
                rhi::fx::ShaderStageData stageData = {.entryPoint = value.substr(0, value.size() - 2)};
                technique.stages.emplace(rhi::fx::ShaderStageType::Vertex, std::move(stageData));
            }
            else if (parameter.compare("pixelShader") == 0)
            {
                rhi::fx::ShaderStageData stageData = {.entryPoint = value.substr(0, value.size() - 2)};
                technique.stages.emplace(rhi::fx::ShaderStageType::Pixel, std::move(stageData));
            }
            else if (parameter.compare("computeShader") == 0)
            {
                rhi::fx::ShaderStageData stageData = {.entryPoint = value.substr(0, value.size() - 2)};
                technique.stages.emplace(rhi::fx::ShaderStageType::Compute, std::move(stageData));
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
                    return false;
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
                    return false;
                }
            }
            else if (parameter.compare("cullSide") == 0)
            {
                if (value.compare("\"back\"") == 0)
                {
                    technique.cullSide = rhi::fx::ShaderCullSide::Back;
                }
                else if (value.compare("\"front\"") == 0)
                {
                    technique.cullSide = rhi::fx::ShaderCullSide::Front;
                }
                else if (value.compare("\"none\"") == 0)
                {
                    technique.cullSide = rhi::fx::ShaderCullSide::None;
                }
                else
                {
                    return false;
                }
            }
        }

        auto beginShaderCode = shaderCode.substr(0, techniqueOffset - 1);
        auto endShaderCode = shaderCode.substr(offset + 1, std::string::npos);
        shaderCode = beginShaderCode + endShaderCode;
        return true;
    }

    auto FXCompiler::generateShaderBindlessData(std::string& shaderCode,
                                                std::span<rhi::fx::ShaderConstantData const> const constants,
                                                std::span<rhi::fx::ShaderStructureData const> const structures,
                                                rhi::fx::ShaderTechniqueData const& technique) -> bool
    {
        for (auto const& [stageType, stageInfo] : technique.stages)
        {
            uint64_t offset = 0;
            offset = shaderCode.find(stageInfo.entryPoint, offset);

            uint64_t const brOpenOffset = shaderCode.find("{", offset);
            uint64_t brCloseOffset = 0;

            for (auto const& constant : constants)
            {
                offset = shaderCode.find(constant.constantName, brOpenOffset);
                brCloseOffset = shaderCode.find("}", brOpenOffset);

                if (offset == std::string::npos || offset >= brCloseOffset)
                {
                    continue;
                }

                do
                {
                    brCloseOffset = shaderCode.find("}", offset);

                    uint64_t const nameOffset = shaderCode.find(constant.constantName, offset);
                    if (nameOffset == std::string::npos)
                    {
                        break;
                    }

                    std::string const member(shaderCode.begin() + nameOffset,
                                             shaderCode.begin() + nameOffset + constant.constantName.size());

                    switch (constant.constantType)
                    {
                        case rhi::fx::ShaderElementType::ConstantBuffer: {
                            uint64_t const dotOffset = shaderCode.find(".", nameOffset);
                            uint64_t constantMemberEndOffset = 0;
                            std::string constantMember;
                            bool isFound = false;

                            auto const& structure = structures[constant.structure];
                            for (auto const& element : structure.elements)
                            {
                                constantMemberEndOffset = shaderCode.find(".", dotOffset + 1);
                                if (constantMemberEndOffset != std::string::npos)
                                {
                                    constantMember = std::string(shaderCode.begin() + dotOffset + 1,
                                                                 shaderCode.begin() + constantMemberEndOffset);
                                    if (element.elementName.compare(constantMember) == 0)
                                    {
                                        isFound = true;
                                        break;
                                    }
                                }

                                constantMemberEndOffset = shaderCode.find(",", dotOffset);
                                if (constantMemberEndOffset != std::string::npos)
                                {
                                    constantMember = std::string(shaderCode.begin() + dotOffset + 1,
                                                                 shaderCode.begin() + constantMemberEndOffset);
                                    if (element.elementName.compare(constantMember) == 0)
                                    {
                                        isFound = true;
                                        break;
                                    }
                                }

                                constantMemberEndOffset = shaderCode.find(")", dotOffset);
                                if (constantMemberEndOffset != std::string::npos)
                                {
                                    constantMember = std::string(shaderCode.begin() + dotOffset + 1,
                                                                 shaderCode.begin() + constantMemberEndOffset);
                                    if (element.elementName.compare(constantMember) == 0)
                                    {
                                        isFound = true;
                                        break;
                                    }
                                }
                            }

                            if (!isFound)
                            {
                                return false;
                            }

                            auto it = std::find_if(
                                structure.elements.begin(), structure.elements.end(),
                                [&](auto const& element) { return element.elementName == constantMember; });

                            if (it == structure.elements.end())
                            {
                                return false;
                            }

                            auto beginShader = shaderCode.substr(0, nameOffset);
                            auto endShader = shaderCode.substr(constantMemberEndOffset, std::string::npos);
                            std::stringstream generatedShader;

                            switch (it->elementType)
                            {
                                case rhi::fx::ShaderElementType::Texture2D: {
                                    generatedShader
                                        << "Texture2D(ResourceDescriptorHeap[NonUniformResourceIndex(ConstantBuffer<"
                                        << structure.structureName << ">(ResourceDescriptorHeap[shaderData." << member
                                        << "])." << constantMember << ")])";
                                    break;
                                }
                                case rhi::fx::ShaderElementType::SamplerState: {
                                    generatedShader
                                        << "SamplerState(SamplerDescriptorHeap[NonUniformResourceIndex(ConstantBuffer<"
                                        << structure.structureName << ">(ResourceDescriptorHeap[shaderData." << member
                                        << "])." << constantMember << ")])";
                                    break;
                                }
                                default: {
                                    generatedShader << "ConstantBuffer<" << structure.structureName
                                                    << ">(ResourceDescriptorHeap[shaderData." << member << "])."
                                                    << constantMember;
                                    break;
                                }
                            }

                            offset = nameOffset + 1 + generatedShader.str().size();
                            shaderCode = beginShader + generatedShader.str() + endShader;
                            break;
                        }
                        case rhi::fx::ShaderElementType::SamplerState: {
                            auto beginShader = shaderCode.substr(0, nameOffset);
                            auto endShader = shaderCode.substr(nameOffset + member.size(), std::string::npos);
                            std::stringstream generatedShader;
                            generatedShader << "SamplerState(SamplerDescriptorHeap[shaderData." << member << "])";

                            offset = nameOffset + 1 + generatedShader.str().size();
                            shaderCode = beginShader + generatedShader.str() + endShader;
                            break;
                        }
                        case rhi::fx::ShaderElementType::Texture2D: {
                            auto beginShader = shaderCode.substr(0, nameOffset);
                            auto endShader = shaderCode.substr(nameOffset + member.size(), std::string::npos);
                            std::stringstream generatedShader;
                            generatedShader << "Texture2D(ResourceDescriptorHeap[shaderData." << member << "])";

                            offset = nameOffset + 1 + generatedShader.str().size();
                            shaderCode = beginShader + generatedShader.str() + endShader;
                            break;
                        }
                        default: {
                            auto beginShader = shaderCode.substr(0, nameOffset);
                            auto endShader = shaderCode.substr(nameOffset + member.size(), std::string::npos);
                            std::stringstream generatedShader;
                            generatedShader << "shaderData." << member;

                            offset = nameOffset + 1 + generatedShader.str().size();
                            shaderCode = beginShader + generatedShader.str() + endShader;
                            break;
                        }
                    }

                } while (offset < brCloseOffset - 1);
            }
        }
        return true;
    }
} // namespace ionengine::tools::shaderc