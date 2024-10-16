// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "dxc.hpp"
#include "precompiled.h"
#include "shadersys/lexer.hpp"
#include "shadersys/parser.hpp"

namespace ionengine::shadersys
{
    auto throwIfFailed(HRESULT const hr) -> void
    {
        if (FAILED(hr))
        {
            throw core::runtime_error(std::format("The program closed with an error {:04x}", hr));
        }
    }

    auto isDefaultSemantic(std::string_view const semantic) -> bool
    {
        return semantic.find("SV_") != std::string_view::npos;
    }

    DXCCompiler::DXCCompiler(fx::APIType const apiType) : apiType(apiType)
    {
        throwIfFailed(::DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), compiler.put_void()));
        throwIfFailed(::DxcCreateInstance(CLSID_DxcUtils, __uuidof(IDxcUtils), utils.put_void()));
        throwIfFailed(utils->CreateDefaultIncludeHandler(includeHandler.put()));
    }

    auto DXCCompiler::compileFromFile(std::filesystem::path const& filePath,
                                      std::string& errors) -> std::optional<ShaderFile>
    {
        std::basic_ifstream<uint8_t> stream(filePath);
        if (!stream.is_open())
        {
            errors = "the input file is in a different format, is corrupted, or was not found";
            return std::nullopt;
        }

        std::basic_string<uint8_t> buffer = {std::istreambuf_iterator<uint8_t>(stream.rdbuf()), {}};

        Lexer lexer(std::string_view(reinterpret_cast<char const*>(buffer.data()), buffer.size()));
        Parser parser;

        fx::HeaderData headerData;
        fx::OutputData outputData;
        std::unordered_map<shadersys::fx::StageType, std::string> stageData;
        fx::StructureData materialData;

        try
        {
            parser.parse(lexer, headerData, outputData, stageData, materialData);
        }
        catch (parser_error e)
        {
            errors = e.what();
            return std::nullopt;
        }

        std::basic_stringstream<uint8_t> streambuf;

        fx::ShaderData shaderData{.headerData = std::move(headerData), .outputData = std::move(outputData)};

        std::vector<std::string> permutations = {"BASE"};

        if (shaderData.headerData.domain.compare("Surface") == 0)
        {
            permutations.emplace_back("FEATURE_SKINNING");
        }

        for (size_t const i : std::views::iota(0u, permutations.size()))
        {
            shaderData.permutations[permutations[i]] = 1 << i;
        }

        auto shaderVariants = getAllVariants(permutations, shaderData.permutations);
        for (auto const variantFlags : shaderVariants)
        {
            fx::ShaderVariantData shaderVariantData{};

            if (materialData.size > 0)
            {
                shaderVariantData.constants.emplace_back(shadersys::common::materialConstantData);
                shaderVariantData.structures.emplace_back(std::move(materialData));
            }

            if (shaderData.headerData.domain.compare("Surface") == 0)
            {
                shaderVariantData.constants.emplace_back(shadersys::common::transformConstantData);
                shaderVariantData.structures.emplace_back(shadersys::common::transformStructureData);
            }

            for (auto const& [stageType, shaderCode] : stageData)
            {
                fx::StageData shaderStageData{.buffer = static_cast<uint32_t>(shaderData.buffers.size()),
                                              .entryPoint = "main"};

                std::wstring const defaultIncludePath = L"-I " + filePath.parent_path().wstring();
                std::vector<LPCWSTR> arguments = {L"-E main", defaultIncludePath.c_str(), L"-HV 2021"};

                switch (stageType)
                {
                    case fx::StageType::Vertex: {
                        arguments.emplace_back(L"-T vs_6_6");
                        break;
                    }
                    case fx::StageType::Pixel: {
                        arguments.emplace_back(L"-T ps_6_6");
                        break;
                    }
                    case fx::StageType::Compute: {
                        arguments.emplace_back(L"-T cs_6_6");
                        break;
                    }
                }

                if (shaderData.headerData.domain.compare("Screen") == 0)
                {
                    arguments.emplace_back(L"-D SHADER_DOMAIN_TYPE_SCREEN");
                }
                else if (shaderData.headerData.domain.compare("Surface") == 0)
                {
                    arguments.emplace_back(L"-D SHADER_DOMAIN_TYPE_SURFACE");
                }

                DxcBuffer shaderBuffer{.Ptr = shaderCode.data(), .Size = shaderCode.size(), .Encoding = DXC_CP_UTF8};

                winrt::com_ptr<IDxcResult> result;
                throwIfFailed(compiler->Compile(&shaderBuffer, arguments.data(),
                                                static_cast<uint32_t>(arguments.size()), includeHandler.get(),
                                                __uuidof(IDxcResult), result.put_void()));

                winrt::com_ptr<IDxcBlobUtf8> errorsBlob;
                throwIfFailed(
                    result->GetOutput(DXC_OUT_ERRORS, __uuidof(IDxcBlobUtf8), errorsBlob.put_void(), nullptr));

                if (errorsBlob && errorsBlob->GetStringLength() > 0)
                {
                    errors = std::string(reinterpret_cast<char*>(errorsBlob->GetBufferPointer()),
                                         errorsBlob->GetBufferSize());
                    return std::nullopt;
                }

                winrt::com_ptr<IDxcBlob> shaderBlob;
                throwIfFailed(result->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), shaderBlob.put_void(), nullptr));

                fx::BufferData bufferData{.offset = static_cast<uint64_t>(streambuf.tellp()),
                                          .size = shaderBlob->GetBufferSize()};
                shaderData.buffers.emplace_back(std::move(bufferData));

                streambuf.write(reinterpret_cast<uint8_t const*>(shaderBlob->GetBufferPointer()),
                                shaderBlob->GetBufferSize());

                winrt::com_ptr<IDxcBlob> reflectionBlob;
                throwIfFailed(
                    result->GetOutput(DXC_OUT_REFLECTION, __uuidof(IDxcBlob), reflectionBlob.put_void(), nullptr));

                DxcBuffer reflectionBuffer{.Ptr = reflectionBlob->GetBufferPointer(),
                                           .Size = reflectionBlob->GetBufferSize()};

                winrt::com_ptr<ID3D12ShaderReflection> shaderReflection;
                throwIfFailed(utils->CreateReflection(&reflectionBuffer, __uuidof(ID3D12ShaderReflection),
                                                      shaderReflection.put_void()));

                D3D12_SHADER_DESC shaderDesc{};
                throwIfFailed(shaderReflection->GetDesc(&shaderDesc));

                size_t inputSize = 0;

                for (uint32_t const i : std::views::iota(0u, shaderDesc.InputParameters))
                {
                    D3D12_SIGNATURE_PARAMETER_DESC signatureParameterDesc{};
                    throwIfFailed(shaderReflection->GetInputParameterDesc(i, &signatureParameterDesc));

                    if (isDefaultSemantic(signatureParameterDesc.SemanticName))
                    {
                        continue;
                    }

                    fx::VertexFormat format;
                    if (signatureParameterDesc.Mask == 1)
                    {
                        switch (signatureParameterDesc.ComponentType)
                        {
                            case D3D_REGISTER_COMPONENT_UINT32:
                                format = fx::VertexFormat::R32_UINT;
                                break;
                            case D3D_REGISTER_COMPONENT_SINT32:
                                format = fx::VertexFormat::R32_SINT;
                                break;
                            case D3D_REGISTER_COMPONENT_FLOAT32:
                                format = fx::VertexFormat::R32_FLOAT;
                                break;
                        }
                    }
                    else if (signatureParameterDesc.Mask <= 3)
                    {
                        switch (signatureParameterDesc.ComponentType)
                        {
                            case D3D_REGISTER_COMPONENT_UINT32:
                                format = fx::VertexFormat::RG32_UINT;
                                break;
                            case D3D_REGISTER_COMPONENT_SINT32:
                                format = fx::VertexFormat::RG32_UINT;
                                break;
                            case D3D_REGISTER_COMPONENT_FLOAT32:
                                format = fx::VertexFormat::RG32_UINT;
                                break;
                        }
                    }
                    else if (signatureParameterDesc.Mask <= 7)
                    {
                        switch (signatureParameterDesc.ComponentType)
                        {
                            case D3D_REGISTER_COMPONENT_UINT32:
                                format = fx::VertexFormat::RGB32_UINT;
                                break;
                            case D3D_REGISTER_COMPONENT_SINT32:
                                format = fx::VertexFormat::RGB32_UINT;
                                break;
                            case D3D_REGISTER_COMPONENT_FLOAT32:
                                format = fx::VertexFormat::RGB32_UINT;
                                break;
                        }
                    }
                    else if (signatureParameterDesc.Mask <= 15)
                    {
                        switch (signatureParameterDesc.ComponentType)
                        {
                            case D3D_REGISTER_COMPONENT_UINT32:
                                format = fx::VertexFormat::RGBA32_UINT;
                                break;
                            case D3D_REGISTER_COMPONENT_SINT32:
                                format = fx::VertexFormat::RGBA32_UINT;
                                break;
                            case D3D_REGISTER_COMPONENT_FLOAT32:
                                format = fx::VertexFormat::RGBA32_UINT;
                                break;
                        }
                    }

                    fx::VertexLayoutElementData elementData{.format = format,
                                                            .semantic =
                                                                signatureParameterDesc.SemanticName +
                                                                std::to_string(signatureParameterDesc.SemanticIndex)};
                    shaderStageData.vertexLayout.elements.emplace_back(std::move(elementData));

                    inputSize += fx::sizeof_VertexFormat(format);
                }

                shaderStageData.vertexLayout.size = inputSize;

                shaderVariantData.stages[stageType] = std::move(shaderStageData);
            }

            shaderData.shaders[variantFlags] = std::move(shaderVariantData);
        }

        return ShaderFile{.magic = fx::Magic,
                          .apiType = apiType,
                          .shaderData = std::move(shaderData),
                          .blob = {std::istreambuf_iterator<uint8_t>(streambuf.rdbuf()), {}}};
    }
} // namespace ionengine::shadersys