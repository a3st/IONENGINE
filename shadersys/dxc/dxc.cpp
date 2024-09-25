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
    }

    auto DXCCompiler::compileFromBytes(std::span<uint8_t const> const dataBytes) -> std::optional<fx::ShaderEffectFile>
    {
        return this->compileBufferData(dataBytes);
    }

    auto DXCCompiler::compileFromFile(std::filesystem::path const& filePath) -> std::optional<fx::ShaderEffectFile>
    {
        std::basic_ifstream<uint8_t> stream(filePath);
        if (!stream.is_open())
        {
            return std::nullopt;
        }

        std::basic_string<uint8_t> buffer = {std::istreambuf_iterator<uint8_t>(stream.rdbuf()), {}};
        return this->compileBufferData(buffer);
    }

    auto DXCCompiler::compileBufferData(std::span<uint8_t const> const buffer) -> std::optional<fx::ShaderEffectFile>
    {
        Lexer lexer(std::string_view(reinterpret_cast<char const*>(buffer.data()), buffer.size()));
        Parser parser;

        fx::ShaderHeaderData headerData;
        fx::ShaderOutputData outputData;
        std::unordered_map<shadersys::fx::ShaderStageType, std::string> stageData;
        parser.parse(lexer, headerData, outputData, stageData);

        

        return std::nullopt;
    }
} // namespace ionengine::shadersys