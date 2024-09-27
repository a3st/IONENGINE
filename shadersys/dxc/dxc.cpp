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

    DXCCompiler::DXCCompiler(fx::ShaderAPIType const apiType)
    {
        throwIfFailed(::DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), compiler.put_void()));
        throwIfFailed(::DxcCreateInstance(CLSID_DxcUtils, __uuidof(IDxcUtils), utils.put_void()));
        throwIfFailed(utils->CreateDefaultIncludeHandler(includeHandler.put()));
    }

    auto DXCCompiler::compileFromFile(std::filesystem::path const& filePath) -> std::optional<fx::ShaderEffectFile>
    {
        std::basic_ifstream<uint8_t> stream(filePath);
        if (!stream.is_open())
        {
            return std::nullopt;
        }

        std::basic_string<uint8_t> buffer = {std::istreambuf_iterator<uint8_t>(stream.rdbuf()), {}};

        Lexer lexer(std::string_view(reinterpret_cast<char const*>(buffer.data()), buffer.size()));
        Parser parser;

        fx::ShaderHeaderData headerData;
        fx::ShaderOutputData outputData;
        std::unordered_map<shadersys::fx::ShaderStageType, std::string> stageData;

        parser.parse(lexer, headerData, outputData, stageData);

        for (auto const& [stageType, shaderCode] : stageData)
        {
            std::wstring const defaultIncludePath = L"-I " + filePath.parent_path().wstring();
            std::vector<LPCWSTR> arguments = {L"-E main", defaultIncludePath.c_str()};

            switch (stageType)
            {
                case fx::ShaderStageType::Vertex: {
                    arguments.emplace_back(L"-T vs_6_6");
                    break;
                }
                case fx::ShaderStageType::Pixel: {
                    arguments.emplace_back(L"-T ps_6_6");
                    break;
                }
                case fx::ShaderStageType::Compute: {
                    arguments.emplace_back(L"-T cs_6_6");
                    break;
                }
            }

            if (headerData.shaderDomain.compare("Screen") == 0)
            {
                arguments.emplace_back(L"-D SHADER_DOMAIN_TYPE_SCREEN");
            }

            DxcBuffer shaderBuffer{.Ptr = shaderCode.data(), .Size = shaderCode.size(), .Encoding = DXC_CP_UTF8};

            winrt::com_ptr<IDxcResult> result;
            throwIfFailed(compiler->Compile(&shaderBuffer, arguments.data(), static_cast<uint32_t>(arguments.size()),
                                            includeHandler.get(), __uuidof(IDxcResult), result.put_void()));

            winrt::com_ptr<IDxcBlobUtf8> errorsBlob;
            throwIfFailed(result->GetOutput(DXC_OUT_ERRORS, __uuidof(IDxcBlobUtf8), errorsBlob.put_void(), nullptr));

            if (errorsBlob && errorsBlob->GetStringLength() > 0)
            {
                std::cerr << "\n" + std::string(reinterpret_cast<char*>(errorsBlob->GetBufferPointer()),
                                                errorsBlob->GetBufferSize())
                          << std::endl;
                return std::nullopt;
            }

            winrt::com_ptr<IDxcBlob> reflectionBlob;
            throwIfFailed(result->GetOutput(DXC_OUT_REFLECTION, __uuidof(IDxcBlob), reflectionBlob.put_void(), nullptr));

            DxcBuffer reflectionBuffer{.Ptr = reflectionBlob->GetBufferPointer(), .Size = reflectionBlob->GetBufferSize()};

            winrt::com_ptr<ID3D12ShaderReflection> shaderReflection;
            throwIfFailed(utils->CreateReflection(&reflectionBuffer, __uuidof(ID3D12ShaderReflection), shaderReflection.put_void()));

            // D3D12_SHADER_INPUT_BIND_DESC shaderInputBindDesc;
            // shaderReflection->GetResourceBindingDesc(0, &shaderInputBindDesc);
        }

        return std::nullopt;
    }
} // namespace ionengine::shadersys