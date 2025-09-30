// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "dxc.hpp"
#include "../common.hpp"
#include "../hlslgen.hpp"
#include "../lexer.hpp"
#include "../parser.hpp"
#include "core/string_utils.hpp"
#include "precompiled.h"

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

    DXCCompiler::DXCCompiler(asset::fx::ShaderFormat const shaderFormat, std::filesystem::path const& includeBasePath)
        : _shaderFormat(shaderFormat), _includeBasePath(includeBasePath)
    {
        throwIfFailed(::DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), _compiler.put_void()));
        throwIfFailed(::DxcCreateInstance(CLSID_DxcUtils, __uuidof(IDxcUtils), _utils.put_void()));
        throwIfFailed(_utils->CreateDefaultIncludeHandler(_includeHandler.put()));
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
            outputData.depthWrite = core::string_utils::lexical_cast<bool>(it->second).value_or(false);
        }
        if ((it = attributes.find("StencilWrite")) != attributes.end())
        {
            outputData.stencilWrite = core::string_utils::lexical_cast<bool>(it->second).value_or(false);
        }
    }

    auto DXCCompiler::getEffectDataCode(asset::fx::StructureData const& structureData) -> std::string
    {
        std::ostringstream oss;
        oss << "struct EffectData { ";

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

    auto DXCCompiler::compile(std::filesystem::path const& filePath) -> std::expected<asset::ShaderFile, core::error>
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

        std::wstring const includeFilePath = L"-I " + filePath.parent_path().wstring();
        std::vector<LPCWSTR> arguments;

        std::string const effectDataCode = this->getEffectDataCode(parseData.effectData);

        std::string inputDataCode;
        HLSLCodeGen generator;
        asset::fx::VertexLayoutData vertexLayout;

        if (parseData.headerData.domain.compare("PostProcess") == 0)
        {
            inputDataCode += generator.getHLSLStruct<inputs::BaseVSInput>("VSInput") + "\n";
            inputDataCode += generator.getHLSLStruct<inputs::BaseVSOutput>("VSOutput") + "\n";
            inputDataCode += generator.getHLSLStruct<inputs::ForwardPSOutput>("PSOutput") + "\n";
            inputDataCode += generator.getHLSLStruct<common::SamplerData>("SamplerData") + "\n";
            inputDataCode += generator.getHLSLStruct<common::PassData>("PassData") + "\n";
            inputDataCode += generator.getHLSLConstBuffer<constants::PostProcessShaderData>("ShaderData", 0, 0) + "\n";

            shaderData.structures.emplace_back(common::SamplerData::structureData);
            shaderData.structures.emplace_back(common::PassData::structureData);
            shaderData.structures.emplace_back(constants::PostProcessShaderData::structureData);

            vertexLayout = {};
        }
        else if (parseData.headerData.domain.compare("Surface") == 0)
        {
            inputDataCode += generator.getHLSLStruct<inputs::SurfaceVSInput>("VSInput") + "\n";
            inputDataCode += generator.getHLSLStruct<inputs::SurfaceVSOutput>("VSOutput") + "\n";
            inputDataCode += generator.getHLSLStruct<inputs::ForwardPSOutput>("PSOutput") + "\n";
            inputDataCode += generator.getHLSLStruct<common::TransformData>("TransformData") + "\n";
            inputDataCode += generator.getHLSLStruct<common::SamplerData>("SamplerData") + "\n";
            inputDataCode += generator.getHLSLConstBuffer<constants::SurfaceShaderData>("ShaderData", 0, 0) + "\n";

            shaderData.structures.emplace_back(common::TransformData::structureData);
            shaderData.structures.emplace_back(common::SamplerData::structureData);
            shaderData.structures.emplace_back(constants::SurfaceShaderData::structureData);

            vertexLayout = inputs::SurfaceVSInput::vertexLayout;
        }
        else if (parseData.headerData.domain.compare("UI") == 0)
        {
            inputDataCode += generator.getHLSLStruct<inputs::UIVSInput>("VSInput") + "\n";
            inputDataCode += generator.getHLSLStruct<inputs::UIVSOutput>("VSOutput") + "\n";
            inputDataCode += generator.getHLSLStruct<inputs::ForwardPSOutput>("PSOutput") + "\n";
            inputDataCode += generator.getHLSLStruct<common::TransformData>("TransformData") + "\n";
            inputDataCode += generator.getHLSLStruct<common::SamplerData>("SamplerData") + "\n";
            inputDataCode += generator.getHLSLConstBuffer<constants::UIShaderData>("ShaderData", 0, 0) + "\n";

            shaderData.structures.emplace_back(common::TransformData::structureData);
            shaderData.structures.emplace_back(common::SamplerData::structureData);
            shaderData.structures.emplace_back(constants::UIShaderData::structureData);

            vertexLayout = inputs::UIVSInput::vertexLayout;
        }

        std::unordered_map<uint32_t, std::string> permutationNames;
        permutationNames[0] = "Default";

        for (uint32_t const i : std::views::iota(0u, parseData.headerData.features.size()))
        {
            permutationNames[i + 1] = parseData.headerData.features[i];
        }

        std::unordered_set<uint32_t> permutationMasks;

        for (uint32_t const mask : std::views::iota(0u, 1u << parseData.headerData.features.size()))
        {
            uint32_t currentPermutation;
            if (mask == 0)
            {
                currentPermutation = 1 << 0;
                std::cout << permutationNames[0];
            }
            else
            {
                currentPermutation = 1 << 0;
                std::cout << permutationNames[0];

                for (uint32_t const j : std::views::iota(0u, parseData.headerData.features.size()))
                {
                    if (mask & (1u << j))
                    {
                        currentPermutation |= 1 << (j + 1);
                        std::cout << " " << permutationNames[j + 1];
                    }
                }
            }

            std::cout << std::endl;

            permutationMasks.emplace(currentPermutation);
        }

        std::cout << "Permutation count: " << permutationMasks.size() << std::endl;

        for (auto const& [stageType, shaderCode] : parseData.shaderData)
        {
            asset::fx::StageData stageData{.buffer = static_cast<uint32_t>(shaderData.buffers.size()),
                                           .entryPoint = "main"};

            arguments.clear();
            arguments.emplace_back(L"-E main");
            arguments.emplace_back(includeFilePath.c_str());
            arguments.emplace_back(_includeBasePath.c_str());
            arguments.emplace_back(L"-HV 2021");
            if (_shaderFormat == asset::fx::ShaderFormat::SPIRV)
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

            // arguments.emplace_back(L"");
            std::wstring defineName;

            for (uint32_t const mask : permutationMasks)
            {
                // defineName = std::format(L"-D USE_{0}", core::string_utils::to_upper_underscore(permutationNames[0]));
                // arguments.back() = defineName.c_str();    
            }

            std::string const stageShaderCode =
                "#include \"shared/internal.hlsli\"\n\n" + effectDataCode + "\n" + inputDataCode + "\n" + shaderCode;

            std::cout << stageShaderCode << std::endl;

            DxcBuffer const codeBuffer{
                .Ptr = stageShaderCode.data(), .Size = stageShaderCode.size(), .Encoding = DXC_CP_UTF8};

            winrt::com_ptr<IDxcResult> compileResult;
            throwIfFailed(_compiler->Compile(&codeBuffer, arguments.data(), static_cast<uint32_t>(arguments.size()),
                                            _includeHandler.get(), __uuidof(IDxcResult), compileResult.put_void()));

            winrt::com_ptr<IDxcBlobUtf8> errorsBlob;
            throwIfFailed(
                compileResult->GetOutput(DXC_OUT_ERRORS, __uuidof(IDxcBlobUtf8), errorsBlob.put_void(), nullptr));

            if (errorsBlob && errorsBlob->GetStringLength() > 0)
            {
                auto errors =
                    std::string(reinterpret_cast<char*>(errorsBlob->GetBufferPointer()), errorsBlob->GetBufferSize());
                return std::unexpected(core::error(errors));
            }

            winrt::com_ptr<IDxcBlob> outBlob;
            throwIfFailed(compileResult->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), outBlob.put_void(), nullptr));

            asset::fx::BufferData bufferData{.offset = static_cast<uint64_t>(shaderBlob.tellp()),
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
                                 .shaderFormat = _shaderFormat,
                                 .shaderData = std::move(shaderData),
                                 .blob = {std::istreambuf_iterator<uint8_t>(shaderBlob.rdbuf()), {}}};
    }
} // namespace ionengine::shadersys