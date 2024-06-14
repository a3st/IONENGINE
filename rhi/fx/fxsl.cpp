// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "fxsl.hpp"
#include "precompiled.h"
#include <simdjson.h>

namespace ionengine::rhi::fx
{
    template <typename Type>
    auto toString(Type const type) -> std::string;

    template <>
    auto toString(ShaderCullSide const type) -> std::string
    {
        switch (type)
        {
            case ShaderCullSide::None: {
                return "none";
            }
            case ShaderCullSide::Back: {
                return "back";
            }
            case ShaderCullSide::Front: {
                return "front";
            }
        }

        throw std::invalid_argument("Passed invalid argument into function");
    }

    template <>
    auto toString(ShaderStageType const type) -> std::string
    {
        switch (type)
        {
            case ShaderStageType::Vertex: {
                return "vertexShader";
            }
            case ShaderStageType::Pixel: {
                return "pixelShader";
            }
            case ShaderStageType::Compute: {
                return "computeShader";
            }
        }

        throw std::invalid_argument("Passed invalid argument into function");
    }

    template <>
    auto toString(ShaderElementType const type) -> std::string
    {
        switch (type)
        {
            case ShaderElementType::Bool: {
                return "BOOL";
            }
            case ShaderElementType::ConstantBuffer: {
                return "CONST_BUFFER";
            }
            case ShaderElementType::Float2: {
                return "FLOAT2";
            }
            case ShaderElementType::Float2x2: {
                return "FLOAT2x2";
            }
            case ShaderElementType::Float3: {
                return "FLOAT3";
            }
            case ShaderElementType::Float3x3: {
                return "FLOAT3x3";
            }
            case ShaderElementType::Float4: {
                return "FLOAT4";
            }
            case ShaderElementType::Float4x4: {
                return "FLOAT4x4";
            }
            case ShaderElementType::Float: {
                return "FLOAT";
            }
            case ShaderElementType::SamplerState: {
                return "SAMPLER_STATE";
            }
            case ShaderElementType::StorageBuffer: {
                return "STORAGE_BUFFER";
            }
            case ShaderElementType::Texture2D: {
                return "TEXTURE_2D";
            }
            case ShaderElementType::Uint: {
                return "UINT";
            }
        }

        throw std::invalid_argument("Passed invalid argument into function");
    }

    template <typename Type>
    auto fromString(std::string_view const type) -> Type;

    template <>
    auto fromString(std::string_view const type) -> ShaderStageType
    {
        if (type.compare("vertexShader") == 0)
        {
            return ShaderStageType::Vertex;
        }
        else if (type.compare("pixelShader") == 0)
        {
            return ShaderStageType::Pixel;
        }
        else if (type.compare("computeShader") == 0)
        {
            return ShaderStageType::Compute;
        }
        else
        {
            throw std::invalid_argument("Passed invalid argument into function");
        }
    }

    template <>
    auto fromString(std::string_view const type) -> ShaderElementType
    {
        if (type.compare("UINT") == 0)
        {
            return ShaderElementType::Uint;
        }
        else if (type.compare("FLOAT") == 0)
        {
            return ShaderElementType::Float;
        }
        else if (type.compare("FLOAT2") == 0)
        {
            return ShaderElementType::Float2;
        }
        else if (type.compare("FLOAT3") == 0)
        {
            return ShaderElementType::Float3;
        }
        else if (type.compare("FLOAT4") == 0)
        {
            return ShaderElementType::Float4;
        }
        else if (type.compare("FLOAT3x3") == 0)
        {
            return ShaderElementType::Float3x3;
        }
        else if (type.compare("FLOAT4x4") == 0)
        {
            return ShaderElementType::Float4x4;
        }
        else if (type.compare("CONST_BUFFER") == 0)
        {
            return ShaderElementType::ConstantBuffer;
        }
        else if (type.compare("SAMPLER_STATE") == 0)
        {
            return ShaderElementType::SamplerState;
        }
        else if (type.compare("STORAGE_BUFFER") == 0)
        {
            return ShaderElementType::StorageBuffer;
        }
        else if (type.compare("TEXTURE_2D") == 0)
        {
            return ShaderElementType::Texture2D;
        }
        else if (type.compare("BOOL") == 0)
        {
            return ShaderElementType::Bool;
        }
        else
        {
            throw std::invalid_argument("Passed invalid argument into function");
        }
    }

    template <>
    auto fromString(std::string_view const type) -> ShaderCullSide
    {
        if (type.compare("none") == 0)
        {
            return ShaderCullSide::None;
        }
        else if (type.compare("back") == 0)
        {
            return ShaderCullSide::Back;
        }
        else if (type.compare("front") == 0)
        {
            return ShaderCullSide::Front;
        }
        else
        {
            throw std::invalid_argument("Passed invalid argument into function");
        }
    }

    auto readJsonChunkData(ShaderEffectData& object, std::vector<uint8_t>& data) -> bool
    {
        simdjson::ondemand::parser parser;
        auto document = parser.iterate(data.data(), data.size(), data.size() + simdjson::SIMDJSON_PADDING);

        auto technique = document["technique"];

        simdjson::fallback::ondemand::value computeShader;
        auto error = technique["computeShader"].get(computeShader);

        std::vector<std::string> stageNames;
        bool isComputeShader = false;

        if (error == simdjson::error_code::SUCCESS)
        {
            stageNames.emplace_back("computeShader");
            isComputeShader = true;
        }
        else
        {
            stageNames.emplace_back("vertexShader");
            stageNames.emplace_back("pixelShader");
        }

        for (auto const& stageName : stageNames)
        {
            simdjson::fallback::ondemand::value shader;
            error = technique[stageName].get(shader);
            if (error != simdjson::error_code::SUCCESS)
            {
                return false;
            }

            int64_t buffer;
            error = shader["buffer"].get_int64().get(buffer);
            if (error != simdjson::error_code::SUCCESS)
            {
                return false;
            }

            std::string_view entryPoint;
            error = shader["entryPoint"].get_string().get(entryPoint);
            if (error != simdjson::error_code::SUCCESS)
            {
                return false;
            }

            ShaderStageData stageData = {.buffer = static_cast<int32_t>(buffer), .entryPoint = std::string(entryPoint)};
            object.technique.stages.emplace(fromString<ShaderStageType>(stageName), stageData);
        }

        if (!isComputeShader)
        {
            bool depthWrite;
            error = technique["depthWrite"].get_bool().get(depthWrite);
            if (error != simdjson::error_code::SUCCESS)
            {
                return false;
            }

            object.technique.depthWrite = depthWrite;

            bool stencilWrite;
            error = technique["stencilWrite"].get_bool().get(stencilWrite);
            if (error != simdjson::error_code::SUCCESS)
            {
                return false;
            }

            object.technique.stencilWrite = stencilWrite;

            std::string_view cullSide;
            error = technique["cullSide"].get_string().get(cullSide);
            if (error != simdjson::error_code::SUCCESS)
            {
                return false;
            }

            object.technique.cullSide = fromString<ShaderCullSide>(cullSide);
        }

        for (auto constant : document["constants"])
        {
            ShaderConstantData constantData = {};

            std::string_view name;
            error = constant["name"].get_string().get(name);
            if (error != simdjson::error_code::SUCCESS)
            {
                return false;
            }

            constantData.name = std::string(name);

            std::string_view constantType;
            error = constant["type"].get_string().get(constantType);
            if (error != simdjson::error_code::SUCCESS)
            {
                return false;
            }

            constantData.constantType = fromString<ShaderElementType>(constantType);

            int64_t structure;
            error = constant["structure"].get_int64().get(structure);
            if (error != simdjson::error_code::SUCCESS)
            {
                return false;
            }

            constantData.structure = static_cast<int32_t>(structure);

            object.constants.emplace_back(std::move(constantData));
        }

        for (auto structure : document["structures"])
        {
            ShaderStructureData structureData = {};

            std::string_view name;
            error = structure["name"].get_string().get(name);
            if (error != simdjson::error_code::SUCCESS)
            {
                return false;
            }

            structureData.name = std::string(name);

            simdjson::fallback::ondemand::value elements;
            error = structure["elements"].get(elements);
            if (error != simdjson::error_code::SUCCESS)
            {
                return false;
            }

            for (auto element : elements)
            {
                ShaderStructureElementData elementData = {};

                error = element["name"].get_string().get(name);
                if (error != simdjson::error_code::SUCCESS)
                {
                    return false;
                }

                elementData.name = std::string(name);

                std::string_view elementType;
                error = element["type"].get_string().get(elementType);
                if (error != simdjson::error_code::SUCCESS)
                {
                    return false;
                }

                elementData.elementType = fromString<ShaderElementType>(elementType);

                std::string_view semantic;
                error = element["semantic"].get_string().get(semantic);
                if (error != simdjson::error_code::SUCCESS)
                {
                    return false;
                }

                elementData.semantic = std::move(semantic);

                structureData.elements.emplace_back(std::move(elementData));
            }

            int64_t size;
            error = structure["sizeInBytes"].get_int64().get(size);
            if (error != simdjson::error_code::SUCCESS)
            {
                return false;
            }

            structureData.size = static_cast<int32_t>(size);

            object.structures.emplace_back(std::move(structureData));
        }
        return true;
    }

    auto generateJsonChunkData(ShaderEffectData const& object) -> std::string
    {
        std::stringstream stream;
        stream << "{\"technique\":{";
        
        for (auto const& [stageType, stageInfo] : object.technique.stages)
        {
            stream << "\"" + toString(stageType) + "\":{\"buffer\":" + std::to_string(stageInfo.buffer) +
                          ",\"entryPoint\":\"" + stageInfo.entryPoint + "\"},";
        }

        stream << "\"depthWrite\":" << std::boolalpha << object.technique.depthWrite
               << ",\"stencilWrite\":" << std::boolalpha << object.technique.stencilWrite
               << ",\"cullSide\":" << "\"" + toString(object.technique.cullSide) + "\"},\"constants\":[";

        bool isFirst = true;

        for (auto const& constant : object.constants)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << "{\"name\":\"" + constant.name + "\",\"type\":\"" + toString(constant.constantType) +
                          "\",\"structure\":" + std::to_string(constant.structure) + "}";

            isFirst = false;
        }
        stream << "],\"structures\":[";

        isFirst = true;

        for (auto const& structure : object.structures)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << "{\"name\":\"" + structure.name + "\",\"elements\":[";

            isFirst = true;

            for (auto const& element : structure.elements)
            {
                if (!isFirst)
                {
                    stream << ",";
                }

                stream << "{\"name\":\"" + element.name + "\",\"type\":\"" + toString(element.elementType) +
                              "\",\"semantic\":\"" + element.semantic + "\"}";

                isFirst = false;
            }

            stream << "],\"sizeInBytes\":" << std::to_string(structure.size) << "}";

            isFirst = false;
        }
        stream << "]}";
        return stream.str();
    }

    /*
    auto FXSL::loadFromMemory(std::span<uint8_t const> const data) -> std::optional<ShaderEffectData>
    {
        std::basic_spanstream<uint8_t> stream(std::span<uint8_t>(const_cast<uint8_t*>(data.data()), data.size()),
                                              std::ios::binary | std::ios::in);

        Header header;
        stream.read((uint8_t*)&header, sizeof(Header));

        if (std::memcmp(header.magic.data(), Magic.data(), Magic.size()) != 0)
        {
            return std::nullopt;
        }

        ChunkHeader chunkHeader;
        stream.read((uint8_t*)&chunkHeader, sizeof(ChunkHeader));

        std::vector<uint8_t> chunkJsonData(chunkHeader.chunkLength);
        stream.read(chunkJsonData.data(), chunkJsonData.size());

        ShaderEffect effect = {};

        if (!readJsonChunkData(effect, chunkJsonData))
        {
            return std::nullopt;
        }

        do
        {
            stream.read((uint8_t*)&chunkHeader, sizeof(ChunkHeader));
            std::vector<uint8_t> buffer(chunkHeader.chunkLength);
            stream.read(buffer.data(), buffer.size());
            effect.buffers.emplace_back(std::move(buffer));
        } while (header.length > stream.tellg());
        return effect;
    }

    auto FXSL::loadFromFile(std::filesystem::path const& filePath) -> std::optional<ShaderEffectData>
    {
        std::basic_fstream<uint8_t> stream(filePath, std::ios::binary | std::ios::in);

        if (!stream.is_open())
        {
            return std::nullopt;
        }

        Header header;
        stream.read((uint8_t*)&header, sizeof(Header));

        if (std::memcmp(header.magic.data(), Magic.data(), Magic.size()) != 0)
        {
            return std::nullopt;
        }

        ChunkHeader chunkHeader;
        stream.read((uint8_t*)&chunkHeader, sizeof(ChunkHeader));

        std::vector<uint8_t> chunkJsonData(chunkHeader.chunkLength);
        stream.read(chunkJsonData.data(), chunkJsonData.size());

        ShaderEffectData effect = {};

        if (!readJsonChunkData(effect, chunkJsonData))
        {
            return std::nullopt;
        }

        do
        {
            stream.read((uint8_t*)&chunkHeader, sizeof(ChunkHeader));
            std::vector<uint8_t> buffer(chunkHeader.chunkLength);
            stream.read(buffer.data(), buffer.size());
            effect.buffers.emplace_back(std::move(buffer));
        } while (header.length > stream.tellg());
        return effect;
    }

    auto FXSL::save(ShaderEffectData const& object, std::filesystem::path const& filePath) -> bool
    {
        std::string const json = generateJsonChunkData(object);

        std::basic_fstream<uint8_t> stream(filePath, std::ios::binary | std::ios::out);
        if (!stream.is_open())
        {
            return false;
        }

        Header header = {.magic = Magic, .length = 0, .target = static_cast<uint32_t>(object.target)};
        stream.write(reinterpret_cast<uint8_t*>(&header), sizeof(Header));
        ChunkHeader json_chunk = {.chunkType = static_cast<uint32_t>(ChunkType::JSON),
                                  .chunkLength = static_cast<uint32_t>(json.size())};
        stream.write(reinterpret_cast<uint8_t const*>(&json_chunk), sizeof(ChunkHeader));
        stream.write(reinterpret_cast<uint8_t const*>(json.data()), json.size());
        for (auto& buffer : object.buffers)
        {
            ChunkHeader binary_chunk = {.chunkType = static_cast<uint32_t>(ChunkType::BIN),
                                        .chunkLength = static_cast<uint32_t>(buffer.size())};
            stream.write(reinterpret_cast<uint8_t const*>(&binary_chunk), sizeof(ChunkHeader));
            stream.write(reinterpret_cast<uint8_t const*>(buffer.data()), buffer.size());
        }
        uint64_t const offset = stream.tellg();
        header.length = offset;
        stream.seekg(0, std::ios::beg);
        stream.write(reinterpret_cast<uint8_t*>(&header), sizeof(Header));
        return true;
    }

    auto FXSL::save(ShaderEffectData const& object, std::basic_stringstream<uint8_t>& stream) -> bool
    {
        std::string const json = generateJsonChunkData(object);

        Header header = {.magic = Magic, .length = 0, .target = static_cast<uint32_t>(object.target)};
        stream.write(reinterpret_cast<uint8_t*>(&header), sizeof(Header));
        ChunkHeader json_chunk = {.chunkType = static_cast<uint32_t>(ChunkType::JSON),
                                  .chunkLength = static_cast<uint32_t>(json.size())};
        stream.write(reinterpret_cast<uint8_t const*>(&json_chunk), sizeof(ChunkHeader));
        stream.write(reinterpret_cast<uint8_t const*>(json.data()), json.size());
        for (auto& buffer : object.buffers)
        {
            ChunkHeader binary_chunk = {.chunkType = static_cast<uint32_t>(ChunkType::BIN),
                                        .chunkLength = static_cast<uint32_t>(buffer.size())};
            stream.write(reinterpret_cast<uint8_t const*>(&binary_chunk), sizeof(ChunkHeader));
            stream.write(reinterpret_cast<uint8_t const*>(buffer.data()), buffer.size());
        }
        uint64_t const offset = stream.tellg();
        header.length = offset;
        stream.seekg(0, std::ios::beg);
        stream.write(reinterpret_cast<uint8_t*>(&header), sizeof(Header));
        return true;
    }
    */

    namespace internal
    {
        auto deserialize(std::basic_istream<uint8_t>& stream) -> std::optional<rhi::fx::ShaderEffectData>
        {
            Header header;
            stream.read((uint8_t*)&header, sizeof(Header));

            if (std::memcmp(header.magic.data(), Magic.data(), Magic.size()) != 0)
            {
                return std::nullopt;
            }

            ChunkHeader chunkHeader;
            stream.read((uint8_t*)&chunkHeader, sizeof(ChunkHeader));

            std::vector<uint8_t> chunkJsonData(chunkHeader.chunkLength);
            stream.read(chunkJsonData.data(), chunkJsonData.size());

            ShaderEffectData effect = {};

            if (!readJsonChunkData(effect, chunkJsonData))
            {
                return std::nullopt;
            }

            do
            {
                stream.read((uint8_t*)&chunkHeader, sizeof(ChunkHeader));
                std::vector<uint8_t> buffer(chunkHeader.chunkLength);
                stream.read(buffer.data(), buffer.size());
                effect.buffers.emplace_back(std::move(buffer));
            } while (header.length > stream.tellg());

            return effect;
        }

        auto serialize(rhi::fx::ShaderEffectData const& object, std::basic_ostream<uint8_t>& stream) -> size_t
        {
            std::string const jsonData = generateJsonChunkData(object);

            Header header = {.magic = Magic, .length = 0, .target = static_cast<uint32_t>(object.target)};
            stream.write(reinterpret_cast<uint8_t*>(&header), sizeof(Header));
            ChunkHeader jsonChunkHeader = {.chunkType = static_cast<uint32_t>(ChunkType::JSON),
                                           .chunkLength = static_cast<uint32_t>(jsonData.size())};
            stream.write(reinterpret_cast<uint8_t const*>(&jsonChunkHeader), sizeof(ChunkHeader));
            stream.write(reinterpret_cast<uint8_t const*>(jsonData.data()), jsonData.size());
            for (auto& buffer : object.buffers)
            {
                ChunkHeader binaryChunkHeader = {.chunkType = static_cast<uint32_t>(ChunkType::BIN),
                                                 .chunkLength = static_cast<uint32_t>(buffer.size())};
                stream.write(reinterpret_cast<uint8_t const*>(&binaryChunkHeader), sizeof(ChunkHeader));
                stream.write(reinterpret_cast<uint8_t const*>(buffer.data()), buffer.size());
            }
            uint64_t const offset = stream.tellp();
            header.length = offset;
            stream.seekp(0, std::ios::beg);
            stream.write(reinterpret_cast<uint8_t*>(&header), sizeof(Header));
            return offset;
        }
    } // namespace internal
} // namespace ionengine::rhi::fx