// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "dxc.hpp"
#include "../common.hpp"
#include "../hlslgen.hpp"
#include "../lexer.hpp"
#include "../parser.hpp"
#include "core/string_utils.hpp"
#include "precompiled.h"
#include <xxhash.h>

namespace ionengine::shadersys
{
    auto throwIfFailed(HRESULT const hr) -> void
    {
        if (FAILED(hr))
        {
            LPSTR messageText = nullptr;

            ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                 FORMAT_MESSAGE_IGNORE_INSERTS,
                             nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageText, 0, nullptr);

            std::string errorMessage = std::format("The program closed with an error: 0x{:08X} - {}", hr, messageText);

            if (messageText != nullptr)
            {
                errorMessage += std::format("- {}", messageText);
                ::LocalFree(messageText);
            }

            throw std::runtime_error(errorMessage);
        }
    }

    DXCCompiler::DXCCompiler(asset::fx::ShaderFormat const shaderFormat, std::filesystem::path const& includeBasePath)
        : _shaderFormat(shaderFormat), _includeBasePath(includeBasePath)
    {
        throwIfFailed(::DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), _compiler.put_void()));
        throwIfFailed(::DxcCreateInstance(CLSID_DxcUtils, __uuidof(IDxcUtils), _utils.put_void()));
        throwIfFailed(_utils->CreateDefaultIncludeHandler(_includeHandler.put()));
    }

    auto DXCCompiler::getOutputStates(std::unordered_map<std::string, std::string> const& attributes,
                                      asset::fx::OutputData& output) const -> void
    {
        std::unordered_map<std::string, std::string>::const_iterator it;
        if ((it = attributes.find("FillMode")) != attributes.end())
        {
            output.fillMode =
                core::deserialize<core::serialize_ienum, asset::fx::FillMode>(std::istringstream(it->second))
                    .value_or(asset::fx::FillMode::Solid);
        }
        if ((it = attributes.find("CullMode")) != attributes.end())
        {
            output.cullMode =
                core::deserialize<core::serialize_ienum, asset::fx::CullMode>(std::istringstream(it->second))
                    .value_or(asset::fx::CullMode::Back);
        }
        if ((it = attributes.find("DepthWrite")) != attributes.end())
        {
            output.depthWrite = core::string_utils::lexical_cast<bool>(it->second).value_or(false);
        }
        if ((it = attributes.find("StencilWrite")) != attributes.end())
        {
            output.stencilWrite = core::string_utils::lexical_cast<bool>(it->second).value_or(false);
        }
    }

    auto DXCCompiler::getDomainStructures(asset::fx::HeaderData const& header,
                                          asset::fx::StructureData const& effectStructure,
                                          std::span<std::string const> const permutationNames, std::string& shaderCode,
                                          std::vector<asset::fx::StructureData>& structures) const -> bool
    {
        std::unordered_map<std::string, int32_t> defineValues;
        for (auto const& permutationName : permutationNames)
        {
            defineValues[permutationName] = 1;
        }

        HLSLCodeGenerator generator(defineValues);

        std::string structShaderCode;
        asset::fx::StructureData structure;
        std::ostringstream oss;

        generator.getHLSLStruct(effectStructure, structShaderCode);
        oss << structShaderCode << "\n";
        structures.emplace_back(effectStructure);

        if (header.domain.compare("PostProcess") == 0)
        {
            generator.getHLSLStruct<inputs::BaseVSInput>("VSInput", structShaderCode, structure);
            oss << structShaderCode << "\n";
            structures.emplace_back(structure);

            generator.getHLSLStruct<inputs::BaseVSOutput>("VSOutput", structShaderCode, structure);
            oss << structShaderCode << "\n";

            generator.getHLSLStruct<inputs::ForwardPSOutput>("PSOutput", structShaderCode, structure);
            oss << structShaderCode << "\n";

            generator.getHLSLStruct<common::SamplerData>("SamplerData", structShaderCode, structure);
            oss << structShaderCode << "\n";
            structures.emplace_back(structure);

            generator.getHLSLStruct<common::PassData>("PassData", structShaderCode, structure);
            oss << structShaderCode << "\n";
            structures.emplace_back(structure);

            generator.getHLSLConstBuffer<constants::PostProcessShaderData>("ShaderData", 0, 0, structShaderCode,
                                                                           structure);
            oss << structShaderCode << "\n";
            structures.emplace_back(structure);
        }
        else if (header.domain.compare("Surface") == 0)
        {
            generator.getHLSLStruct<inputs::SurfaceVSInput>("VSInput", structShaderCode, structure);
            oss << structShaderCode << "\n";
            structures.emplace_back(structure);

            generator.getHLSLStruct<inputs::SurfaceVSOutput>("VSOutput", structShaderCode, structure);
            oss << structShaderCode << "\n";

            generator.getHLSLStruct<inputs::ForwardPSOutput>("PSOutput", structShaderCode, structure);
            oss << structShaderCode << "\n";

            generator.getHLSLStruct<common::TransformData>("TransformData", structShaderCode, structure);
            oss << structShaderCode << "\n";
            structures.emplace_back(structure);

            generator.getHLSLStruct<common::SamplerData>("SamplerData", structShaderCode, structure);
            oss << structShaderCode << "\n";
            structures.emplace_back(structure);

            generator.getHLSLConstBuffer<constants::SurfaceShaderData>("ShaderData", 0, 0, structShaderCode, structure);
            oss << structShaderCode << "\n";
            structures.emplace_back(structure);
        }
        else if (header.domain.compare("UI") == 0)
        {
            generator.getHLSLStruct<inputs::UIVSInput>("VSInput", structShaderCode, structure);
            oss << structShaderCode << "\n";
            structures.emplace_back(structure);

            generator.getHLSLStruct<inputs::UIVSOutput>("VSOutput", structShaderCode, structure);
            oss << structShaderCode << "\n";

            generator.getHLSLStruct<inputs::ForwardPSOutput>("PSOutput", structShaderCode, structure);
            oss << structShaderCode << "\n";

            generator.getHLSLStruct<common::TransformData>("TransformData", structShaderCode, structure);
            oss << structShaderCode << "\n";
            structures.emplace_back(structure);

            generator.getHLSLStruct<common::SamplerData>("SamplerData", structShaderCode, structure);
            oss << structShaderCode << "\n";
            structures.emplace_back(structure);

            generator.getHLSLConstBuffer<constants::UIShaderData>("ShaderData", 0, 0, structShaderCode, structure);
            oss << structShaderCode << "\n";
            structures.emplace_back(structure);
        }
        else
        {
            return false;
        }

        shaderCode = oss.str();
        return true;
    }

    auto DXCCompiler::getPermutations(asset::fx::HeaderData const& header,
                                      std::unordered_map<uint32_t, std::string>& permutationNames,
                                      std::unordered_set<uint32_t>& permutationMasks) const -> void
    {
        permutationNames[0] = "Default";

        for (uint32_t const i : std::views::iota(0u, header.features.size()))
        {
            permutationNames[i + 1] = header.features[i];
        }

        for (uint32_t const mask : std::views::iota(0u, 1u << header.features.size()))
        {
            uint32_t currentPermutation;
            if (mask == 0)
            {
                currentPermutation = 1 << 0;
            }
            else
            {
                currentPermutation = 1 << 0;

                for (uint32_t const j : std::views::iota(0u, header.features.size()))
                {
                    if (mask & (1u << j))
                    {
                        currentPermutation |= 1 << (j + 1);
                    }
                }
            }

            permutationMasks.emplace(currentPermutation);
        }
    }

    auto DXCCompiler::getPermutationNamesByMask(std::unordered_map<uint32_t, std::string> const& permutationNames,
                                                uint32_t const mask, std::vector<std::string>& outputNames) const
        -> void
    {
        if (mask == 0)
        {
            outputNames.emplace_back(permutationNames.at(0));
        }
        else
        {
            outputNames.emplace_back(permutationNames.at(0));
            for (uint32_t const i : std::views::iota(1u, permutationNames.size()))
            {
                if (mask & (1u << i))
                {
                    outputNames.emplace_back(permutationNames.at(i));
                }
            }
        }
    }

    auto DXCCompiler::compile(std::filesystem::path const& filePath) -> std::expected<asset::ShaderFile, core::error>
    {
        _cacheShaderStages.clear();

        Lexer shaderLexer;
        auto lexerResult = shaderLexer.parse(filePath);
        if (!lexerResult.has_value())
        {
            return std::unexpected(lexerResult.error());
        }

        Parser shaderParser;
        auto parserResult = shaderParser.parse(lexerResult.value());
        if (!parserResult.has_value())
        {
            return std::unexpected(parserResult.error());
        }

        ShaderParseData const shaderParseData = std::move(parserResult.value());

        std::unordered_map<uint32_t, asset::fx::PermutationData> shaderPermutations;
        std::vector<asset::fx::StageData> shaderStages;
        std::vector<asset::fx::BufferData> shaderBuffers;
        std::basic_stringstream<uint8_t> shaderBlob;

        std::unordered_map<uint32_t, std::string> permutationNames;
        std::unordered_set<uint32_t> permutationMasks;
        this->getPermutations(shaderParseData.header, permutationNames, permutationMasks);

        std::wstring const includeFileArgument = L"-I " + filePath.parent_path().wstring();
        std::wstring const includeBaseArgument = L"-I " + _includeBasePath.wstring();

        std::vector<std::wstring> arguments;

        for (uint32_t const permutationMask : permutationMasks)
        {
            std::vector<std::string> stagePermutationNames;
            this->getPermutationNamesByMask(permutationNames, permutationMask, stagePermutationNames);

            std::vector<uint32_t> shaderPermutationStages;
            std::vector<asset::fx::StructureData> shaderPermutationStructures;

            for (auto const& [stageType, shaderCode] : shaderParseData.shaderCodes)
            {
                arguments.clear();

                arguments.emplace_back(L"-E main");
                arguments.emplace_back(includeFileArgument.c_str());
                arguments.emplace_back(includeBaseArgument.c_str());
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

                for (auto const& permutationName : stagePermutationNames)
                {
                    arguments.emplace_back(core::string_utils::to_wstring(
                        std::format("-D USE_{0}", core::string_utils::to_upper_underscore(permutationName))));
                }

                std::string inputShaderCode;
                if (!this->getDomainStructures(shaderParseData.header, shaderParseData.effectStructure,
                                               stagePermutationNames, inputShaderCode, shaderPermutationStructures))
                {
                    return std::unexpected(core::error("Invalid Domain value in HEADER section"));
                }

                std::string const stageShaderCode =
                    "#include \"shared/internal.hlsli\"\n\n" + inputShaderCode + "\n" + shaderCode;

#ifdef _DEBUG
                std::cout << stageShaderCode << std::endl;
#endif

                DxcBuffer const dxcBuffer{
                    .Ptr = stageShaderCode.data(), .Size = stageShaderCode.size(), .Encoding = DXC_CP_UTF8};

                auto pArguments = arguments | std::views::transform([](const std::wstring& x) { return x.c_str(); }) |
                                  std::ranges::to<std::vector<LPCWSTR>>();

                winrt::com_ptr<IDxcResult> compileResult;
                throwIfFailed(_compiler->Compile(&dxcBuffer, pArguments.data(),
                                                 static_cast<uint32_t>(pArguments.size()), _includeHandler.get(),
                                                 __uuidof(IDxcResult), compileResult.put_void()));

                winrt::com_ptr<IDxcBlobUtf8> errorsBlob;
                throwIfFailed(
                    compileResult->GetOutput(DXC_OUT_ERRORS, __uuidof(IDxcBlobUtf8), errorsBlob.put_void(), nullptr));

                if (errorsBlob && errorsBlob->GetStringLength() > 0)
                {
                    std::string const errors(reinterpret_cast<char*>(errorsBlob->GetBufferPointer()),
                                             errorsBlob->GetBufferSize());
                    return std::unexpected(core::error(errors));
                }

                winrt::com_ptr<IDxcBlob> outBlob;
                throwIfFailed(
                    compileResult->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), outBlob.put_void(), nullptr));

                // Calculate stage hash for cache shader stages
                uint64_t const stageHash = ::XXH64(outBlob->GetBufferPointer(), outBlob->GetBufferSize(), 0);

                uint32_t stageIndex = 0;

                auto cacheResult = _cacheShaderStages.find(stageHash);
                if (cacheResult != _cacheShaderStages.end())
                {
                    stageIndex = cacheResult->second;
                }
                else
                {
                    // Create a new shader stage
                    asset::fx::BufferData shaderBufferData{.offset = static_cast<uint64_t>(shaderBlob.tellp()),
                                                           .size = outBlob->GetBufferSize()};
                    shaderBuffers.emplace_back(std::move(shaderBufferData));

                    // Write a new shader into big chunk
                    shaderBlob.write(reinterpret_cast<uint8_t const*>(outBlob->GetBufferPointer()),
                                     outBlob->GetBufferSize());

                    if (stageType == asset::fx::StageType::Vertex)
                    {
                        asset::fx::StageData shaderStageData{.buffer = static_cast<uint32_t>(shaderBuffers.size() - 1),
                                                             .entryPoint = "main",
                                                             .inputStructure = 0};
                        shaderStages.emplace_back(std::move(shaderStageData));
                    }
                    else if (stageType == asset::fx::StageType::Pixel)
                    {
                        asset::fx::OutputData shaderOutputData{};
                        this->getOutputStates(shaderParseData.psAttributes, shaderOutputData);

                        asset::fx::StageData shaderStageData{.buffer = static_cast<uint32_t>(shaderBuffers.size() - 1),
                                                             .entryPoint = "main",
                                                             .output = std::move(shaderOutputData)};
                        shaderStages.emplace_back(std::move(shaderStageData));
                    }

                    _cacheShaderStages[stageHash] = shaderStages.size() - 1;

                    stageIndex = shaderStages.size() - 1;
                }

                shaderPermutationStages.emplace_back(stageIndex);
            }

            asset::fx::PermutationData shaderPermutationData{.stages = std::move(shaderPermutationStages),
                                                             .structures = std::move(shaderPermutationStructures)};
            shaderPermutations[permutationMask] = std::move(shaderPermutationData);
        }

        asset::fx::ShaderData shaderData{.header = shaderParseData.header,
                                         .permutations = std::move(shaderPermutations),
                                         .stages = std::move(shaderStages),
                                         .buffers = std::move(shaderBuffers)};
        return asset::ShaderFile{.magic = asset::fx::Magic,
                                 .shaderFormat = _shaderFormat,
                                 .shaderData = std::move(shaderData),
                                 .shaderBlob = {std::istreambuf_iterator<uint8_t>(shaderBlob.rdbuf()), {}}};
    }
} // namespace ionengine::shadersys