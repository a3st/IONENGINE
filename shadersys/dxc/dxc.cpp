// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "dxc.hpp"
#include "core/string.hpp"
#include "precompiled.h"
#include "shadersys/lexer.hpp"
#include "shadersys/parser.hpp"

namespace ionengine::shadersys
{
    auto throwIfFailed(HRESULT const hr) -> void
    {
        if (FAILED(hr))
        {
            throw std::runtime_error(std::format("the program closed with an error {:04x}", hr));
        }
    }

    auto DXCCompiler::isDefaultSemantic(std::string_view const semantic) -> bool
    {
        return semantic.find("SV_") != std::string_view::npos;
    }

    DXCCompiler::DXCCompiler(asset::fx::ShaderFormat const shaderFormat)
    {
        throwIfFailed(::DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), compiler.put_void()));
        throwIfFailed(::DxcCreateInstance(CLSID_DxcUtils, __uuidof(IDxcUtils), utils.put_void()));
        throwIfFailed(utils->CreateDefaultIncludeHandler(includeHandler.put()));
    }

    auto DXCCompiler::getOutputStates(std::unordered_map<std::string, std::string> const& attributes,
                                      asset::fx::OutputData& outputData) -> void
    {
        std::unordered_map<std::string, std::string>::const_iterator it;
        if ((it = attributes.find("FillMode")) != attributes.end())
        {
            outputData.fillMode =
                core::deserialize<core::serialize_ienum, asset::fx::FillMode>(std::istringstream(it->second))
                    .value_or(asset::fx::FillMode::Solid);
        }
        else if ((it = attributes.find("CullSide")) != attributes.end())
        {
            outputData.cullSide =
                core::deserialize<core::serialize_ienum, asset::fx::CullSide>(std::istringstream(it->second))
                    .value_or(asset::fx::CullSide::Back);
        }
        else if ((it = attributes.find("DepthWrite")) != attributes.end())
        {
            outputData.depthWrite = core::stob(it->second).value_or(false);
        }
        else if ((it = attributes.find("StencilWrite")) != attributes.end())
        {
            outputData.stencilWrite = core::stob(it->second).value_or(false);
        }
    }

    auto DXCCompiler::generateStructureDataCode(asset::fx::StructureData const& structureData, std::string& outCode)
        -> void
    {
        std::ostringstream oss;
        oss << "struct " << structureData.name << " { ";

        for (auto const& element : structureData.elements)
        {
            auto typeResult = core::serialize<core::serialize_oenum, std::ostringstream>(element.type);
            if (!typeResult.has_value())
            {
                continue;
            }

            oss << typeResult.value().str() << " " << element.name << "; ";
        }

        oss << "};";

        outCode = oss.str();
    }

    auto DXCCompiler::getInputAssembler(DxcBuffer const& buffer, asset::fx::VertexLayoutData& vertexLayout) -> void
    {
        winrt::com_ptr<ID3D12ShaderReflection> shaderReflection;
        throwIfFailed(utils->CreateReflection(&buffer, __uuidof(ID3D12ShaderReflection), shaderReflection.put_void()));

        D3D12_SHADER_DESC shaderDesc{};
        throwIfFailed(shaderReflection->GetDesc(&shaderDesc));

        size_t inputSize = 0;

        for (uint32_t const i : std::views::iota(0u, shaderDesc.InputParameters))
        {
            D3D12_SIGNATURE_PARAMETER_DESC signatureParameterDesc{};
            throwIfFailed(shaderReflection->GetInputParameterDesc(i, &signatureParameterDesc));

            if (this->isDefaultSemantic(signatureParameterDesc.SemanticName))
            {
                continue;
            }

            asset::fx::VertexFormat format;
            if (signatureParameterDesc.Mask == 1)
            {
                switch (signatureParameterDesc.ComponentType)
                {
                    case D3D_REGISTER_COMPONENT_UINT32: {
                        format = asset::fx::VertexFormat::R32_UINT;
                        break;
                    }
                    case D3D_REGISTER_COMPONENT_SINT32: {
                        format = asset::fx::VertexFormat::R32_SINT;
                        break;
                    }
                    case D3D_REGISTER_COMPONENT_FLOAT32: {
                        format = asset::fx::VertexFormat::R32_FLOAT;
                        break;
                    }
                }
            }
            else if (signatureParameterDesc.Mask <= 3)
            {
                switch (signatureParameterDesc.ComponentType)
                {
                    case D3D_REGISTER_COMPONENT_UINT32: {
                        format = asset::fx::VertexFormat::RG32_UINT;
                        break;
                    }
                    case D3D_REGISTER_COMPONENT_SINT32: {
                        format = asset::fx::VertexFormat::RG32_SINT;
                        break;
                    }
                    case D3D_REGISTER_COMPONENT_FLOAT32: {
                        format = asset::fx::VertexFormat::RG32_FLOAT;
                        break;
                    }
                }
            }
            else if (signatureParameterDesc.Mask <= 7)
            {
                switch (signatureParameterDesc.ComponentType)
                {
                    case D3D_REGISTER_COMPONENT_UINT32: {
                        format = asset::fx::VertexFormat::RGB32_UINT;
                        break;
                    }
                    case D3D_REGISTER_COMPONENT_SINT32: {
                        format = asset::fx::VertexFormat::RGB32_SINT;
                        break;
                    }
                    case D3D_REGISTER_COMPONENT_FLOAT32: {
                        format = asset::fx::VertexFormat::RGB32_FLOAT;
                        break;
                    }
                }
            }
            else if (signatureParameterDesc.Mask <= 15)
            {
                switch (signatureParameterDesc.ComponentType)
                {
                    case D3D_REGISTER_COMPONENT_UINT32: {
                        format = asset::fx::VertexFormat::RGBA32_UINT;
                        break;
                    }
                    case D3D_REGISTER_COMPONENT_SINT32: {
                        format = asset::fx::VertexFormat::RGBA32_SINT;
                        break;
                    }
                    case D3D_REGISTER_COMPONENT_FLOAT32: {
                        format = asset::fx::VertexFormat::RGBA32_FLOAT;
                        break;
                    }
                }
            }

            asset::fx::VertexLayoutElementData layoutElement = {
                .format = format,
                .semantic = signatureParameterDesc.SemanticName + std::to_string(signatureParameterDesc.SemanticIndex)};
            vertexLayout.elements.emplace_back(layoutElement);

            inputSize += asset::fx::sizeof_VertexFormat(format);
        }
    }

    auto DXCCompiler::compileFromFile(std::filesystem::path const& filePath, std::string& errors)
        -> std::expected<asset::ShaderFile, CompileError>
    {
        Lexer lexer;
        auto lexerResult = lexer.parse(filePath, errors);
        if (!lexerResult.has_value())
        {
            return std::unexpected(CompileError::EOF);
        }

        Parser parser;
        auto parserResult = parser.parse(lexerResult.value(), errors);
        if (!parserResult.has_value())
        {
            return std::unexpected(CompileError::EOF);
        }

        ShaderParseData const parseData = std::move(parserResult.value());

        asset::fx::ShaderData shaderData = {.headerData = parseData.headerData, .structures = {parseData.materialData}};
        std::basic_stringstream<uint8_t> shaderBlob;

        std::wstring const defaultIncludePath = L"-I " + filePath.parent_path().wstring();
        std::vector<LPCWSTR> arguments;

        std::string materialDataCode;
        this->generateStructureDataCode(parseData.materialData, materialDataCode);

        for (auto const& [stageType, shaderCode] : parseData.codeData)
        {
            asset::fx::StageData stageData{.buffer = static_cast<uint32_t>(shaderData.buffers.size()),
                                           .entryPoint = "main"};

            arguments.clear();
            arguments.emplace_back(L"-E main");
            arguments.emplace_back(defaultIncludePath.c_str());
            arguments.emplace_back(L"-HV 2021");

            switch (stageType)
            {
                case asset::fx::StageType::Vertex: {
                    arguments.emplace_back(L"-T vs_6_6");
                    break;
                }
                case asset::fx::StageType::Pixel: {
                    arguments.emplace_back(L"-T ps_6_6");
                    break;
                }
                case asset::fx::StageType::Compute: {
                    arguments.emplace_back(L"-T cs_6_6");
                    break;
                }
            }

            if (parseData.headerData.domain.compare("Screen") == 0)
            {
                arguments.emplace_back(L"-D SHADER_DOMAIN_TYPE_SCREEN");
            }
            else if (parseData.headerData.domain.compare("Surface") == 0)
            {
                arguments.emplace_back(L"-D SHADER_DOMAIN_TYPE_SURFACE");
            }

            std::string const stageShaderCode =
                "#include \"shared/internal.hlsli\"\n" + materialDataCode + "\n" + shaderCode;

            DxcBuffer const codeBuffer = {
                .Ptr = stageShaderCode.data(), .Size = stageShaderCode.size(), .Encoding = DXC_CP_UTF8};

            winrt::com_ptr<IDxcResult> compileResult;
            throwIfFailed(compiler->Compile(&codeBuffer, arguments.data(), static_cast<uint32_t>(arguments.size()),
                                            includeHandler.get(), __uuidof(IDxcResult), compileResult.put_void()));

            winrt::com_ptr<IDxcBlobUtf8> errorsBlob;
            throwIfFailed(
                compileResult->GetOutput(DXC_OUT_ERRORS, __uuidof(IDxcBlobUtf8), errorsBlob.put_void(), nullptr));

            if (errorsBlob && errorsBlob->GetStringLength() > 0)
            {
                errors =
                    std::string(reinterpret_cast<char*>(errorsBlob->GetBufferPointer()), errorsBlob->GetBufferSize());
                return std::unexpected(CompileError::EOF);
            }

            winrt::com_ptr<IDxcBlob> outBlob;
            throwIfFailed(compileResult->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), outBlob.put_void(), nullptr));

            asset::fx::BufferData const bufferData = {.offset = static_cast<uint64_t>(shaderBlob.tellp()),
                                                      .size = outBlob->GetBufferSize()};
            shaderData.buffers.emplace_back(std::move(bufferData));

            shaderBlob.write(reinterpret_cast<uint8_t const*>(outBlob->GetBufferPointer()), outBlob->GetBufferSize());

            switch (stageType)
            {
                case asset::fx::StageType::Vertex: {
                    winrt::com_ptr<IDxcBlob> reflectBlob;
                    throwIfFailed(compileResult->GetOutput(DXC_OUT_REFLECTION, __uuidof(IDxcBlob),
                                                           reflectBlob.put_void(), nullptr));

                    DxcBuffer const reflectBuffer = {.Ptr = reflectBlob->GetBufferPointer(),
                                                     .Size = reflectBlob->GetBufferSize()};

                    asset::fx::VertexLayoutData vertexLayout{};
                    this->getInputAssembler(reflectBuffer, vertexLayout);
                    stageData.vertexLayout = std::move(vertexLayout);
                    break;
                }
                case asset::fx::StageType::Pixel: {
                    asset::fx::OutputData outputData{};
                    this->getOutputStates(parseData.psAttributes, outputData);
                    stageData.output = std::move(outputData);
                    break;
                }
            }

            shaderData.stages[stageType] = std::move(stageData);
        }

        return asset::ShaderFile{.magic = asset::fx::Magic,
                                 .shaderFormat = asset::fx::ShaderFormat::DXIL,
                                 .shaderData = std::move(shaderData),
                                 .blob = {std::istreambuf_iterator<uint8_t>(shaderBlob.rdbuf()), {}}};
    }
} // namespace ionengine::shadersys