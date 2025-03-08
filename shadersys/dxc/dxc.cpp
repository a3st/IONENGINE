// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "dxc.hpp"
#include "core/string_utils.hpp"
#include "precompiled.h"
#include "shadersys/common.hpp"
#include "shadersys/hlslgen.hpp"
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

    DXCCompiler::DXCCompiler(asset::fx::ShaderFormat const shaderFormat) : shaderFormat(shaderFormat)
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
        if ((it = attributes.find("CullMode")) != attributes.end())
        {
            outputData.cullMode =
                core::deserialize<core::serialize_ienum, asset::fx::CullMode>(std::istringstream(it->second))
                    .value_or(asset::fx::CullMode::Back);
        }
        if ((it = attributes.find("DepthWrite")) != attributes.end())
        {
            outputData.depthWrite = core::string_utils::stob(it->second).value_or(false);
        }
        if ((it = attributes.find("StencilWrite")) != attributes.end())
        {
            outputData.stencilWrite = core::string_utils::stob(it->second).value_or(false);
        }
    }

    auto DXCCompiler::getEffectDataCode(asset::fx::StructureData const& structureData) -> std::string
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

        return oss.str();
    }

    auto DXCCompiler::compileFromFile(std::filesystem::path const& filePath)
        -> std::expected<asset::ShaderFile, core::error>
    {
        Lexer lexer;
        auto lexerResult = lexer.parse(filePath);
        if (!lexerResult.has_value())
        {
            return std::unexpected(lexerResult.error());
        }

        Parser parser;
        auto parserResult = parser.parse(lexerResult.value());
        if (!parserResult.has_value())
        {
            return std::unexpected(parserResult.error());
        }

        ShaderParseData const parseData = std::move(parserResult.value());

        asset::fx::ShaderData shaderData{.headerData = parseData.headerData, .structures = {parseData.effectData}};
        std::basic_stringstream<uint8_t> shaderBlob;

        std::wstring const defaultIncludePath = L"-I " + filePath.parent_path().wstring();
        std::vector<LPCWSTR> arguments;

        std::string const materialDataCode = this->getEffectDataCode(parseData.effectData);

        std::string inputDataCode;
        HLSLCodeGen generator;
        asset::fx::VertexLayoutData vertexLayout;

        if (parseData.headerData.domain.compare("Screen") == 0)
        {
            HLSLCodeGen generator;
            inputDataCode += generator.getHLSLStruct<inputs::BaseVSInput>("VS_INPUT") + "\n";
            inputDataCode += generator.getHLSLStruct<inputs::BaseVSOutput>("VS_OUTPUT") + "\n";
            inputDataCode += generator.getHLSLStruct<inputs::BasePSOutput>("PS_OUTPUT") + "\n";
            inputDataCode += generator.getHLSLStruct<common::SAMPLER_DATA>("SAMPLER_DATA") + "\n";
            inputDataCode += generator.getHLSLStruct<common::PASS_DATA>("PASS_DATA") + "\n";
            inputDataCode +=
                generator.getHLSLConstBuffer<constants::ScreenShaderData>("SHADER_DATA", "gShaderData", 0, 0) + "\n";

            shaderData.structures.emplace_back(common::SAMPLER_DATA::structureData);
            shaderData.structures.emplace_back(common::PASS_DATA::structureData);
            shaderData.structures.emplace_back(constants::ScreenShaderData::structureData);

            vertexLayout = {};
        }
        else if (parseData.headerData.domain.compare("Surface") == 0)
        {
            inputDataCode += generator.getHLSLStruct<inputs::StaticVSInput>("VS_INPUT") + "\n";
            inputDataCode += generator.getHLSLStruct<inputs::StaticVSOutput>("VS_OUTPUT") + "\n";
            inputDataCode += generator.getHLSLStruct<inputs::BasePSOutput>("PS_OUTPUT") + "\n";
            inputDataCode += generator.getHLSLStruct<common::TRANSFORM_DATA>("TRANSFORM_DATA") + "\n";
            inputDataCode += generator.getHLSLStruct<common::SAMPLER_DATA>("SAMPLER_DATA") + "\n";
            inputDataCode +=
                generator.getHLSLConstBuffer<constants::SurfaceShaderData>("SHADER_DATA", "gShaderData", 0, 0) + "\n";

            shaderData.structures.emplace_back(common::TRANSFORM_DATA::structureData);
            shaderData.structures.emplace_back(common::SAMPLER_DATA::structureData);
            shaderData.structures.emplace_back(constants::SurfaceShaderData::structureData);

            vertexLayout = inputs::StaticVSInput::vertexLayout;
        }

        for (auto const& [stageType, shaderCode] : parseData.codeData)
        {
            asset::fx::StageData stageData{.buffer = static_cast<uint32_t>(shaderData.buffers.size()),
                                           .entryPoint = "main"};

            arguments.clear();
            arguments.emplace_back(L"-Wno-ignored-attributes");
            arguments.emplace_back(L"-E main");
            arguments.emplace_back(defaultIncludePath.c_str());
            arguments.emplace_back(L"-HV 2021");
            if (shaderFormat == asset::fx::ShaderFormat::SPIRV)
            {
                arguments.emplace_back(L"-spirv");
            }
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

            std::string const stageShaderCode =
                "#include \"shared/internal.hlsli\"\n\n" + materialDataCode + "\n" + inputDataCode + "\n" + shaderCode;

            // std::cout << stageShaderCode << std::endl;

            DxcBuffer const codeBuffer{
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
                return std::unexpected(core::error(core::error_code::compile, errors));
            }

            winrt::com_ptr<IDxcBlob> outBlob;
            throwIfFailed(compileResult->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), outBlob.put_void(), nullptr));

            asset::fx::BufferData const bufferData{.offset = static_cast<uint64_t>(shaderBlob.tellp()),
                                                   .size = outBlob->GetBufferSize()};
            shaderData.buffers.emplace_back(std::move(bufferData));

            shaderBlob.write(reinterpret_cast<uint8_t const*>(outBlob->GetBufferPointer()), outBlob->GetBufferSize());

            if (stageType == asset::fx::StageType::Vertex)
            {
                stageData.vertexLayout = std::move(vertexLayout);
            }
            else if (stageType == asset::fx::StageType::Pixel)
            {
                asset::fx::OutputData outputData{};
                this->getOutputStates(parseData.psAttributes, outputData);
                stageData.output = std::move(outputData);
            }

            shaderData.stages[stageType] = std::move(stageData);
        }

        return asset::ShaderFile{.magic = asset::fx::Magic,
                                 .shaderFormat = shaderFormat,
                                 .shaderData = std::move(shaderData),
                                 .blob = {std::istreambuf_iterator<uint8_t>(shaderBlob.rdbuf()), {}}};
    }
} // namespace ionengine::shadersys