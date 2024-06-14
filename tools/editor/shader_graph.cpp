// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "shader_graph.hpp"
#include "engine/asset.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    auto generateJsonChunkData(ShaderGraphData const& object) -> std::string
    {
        std::stringstream stream;
        stream << "{\"scene\":{\"nodes\":[";

        bool isFirst = true;
        for (auto const& node : object.nodes)
        {
            if (!isFirst)
            {
                stream << ",";
            }

            stream << "{\"id\":" << node.nodeID << ",\"position\":[" << node.posX << "," << node.posY
                   << "],\"userData\":{\"componentID\":" << node.componentID << ",\"options\":{";

            isFirst = true;
            for (auto const [key, value] : node.options)
            {
                if (!isFirst)
                {
                    stream << ",";
                }

                stream << "\"" << key << "\":\"" << value << "\"";
                isFirst = false;
            }
            stream << "}}";
            isFirst = false;
        }

        stream << "}]}";
        return stream.str();
    }

    namespace internal
    {
        auto deserialize(std::basic_istream<uint8_t>& stream) -> std::optional<ShaderGraphData>
        {
            asset::Header header;
            stream.read((uint8_t*)&header, sizeof(asset::Header));

            if (std::memcmp(header.magic.data(), asset::Magic.data(), asset::Magic.size()) != 0)
            {
                return std::nullopt;
            }

            if (std::memcmp(header.fileType.data(), ShaderGraphFileType.data(), ShaderGraphFileType.size()) != 0)
            {
                return std::nullopt;
            }

            asset::ChunkHeader chunkHeader;
            stream.read((uint8_t*)&chunkHeader, sizeof(asset::ChunkHeader));

            ShaderGraphData shaderGraph = {};

            return shaderGraph;
        }

        auto serialize(ShaderGraphData const& object, std::basic_ostream<uint8_t>& stream) -> size_t
        {
            std::string const jsonData = generateJsonChunkData(object);

            asset::Header header = {.magic = asset::Magic, .length = 0, .fileType = ShaderGraphFileType};
            stream.write(reinterpret_cast<uint8_t*>(&header), sizeof(asset::Header));
            asset::ChunkHeader jsonChunkHeader = {.chunkType = static_cast<uint32_t>(asset::ChunkType::JSON),
                                                  .chunkLength = static_cast<uint32_t>(jsonData.size())};
            stream.write(reinterpret_cast<uint8_t const*>(&jsonChunkHeader), sizeof(asset::ChunkHeader));
            stream.write(reinterpret_cast<uint8_t const*>(jsonData.data()), jsonData.size());
            uint64_t const offset = stream.tellp();
            header.length = offset;
            stream.seekp(0, std::ios::beg);
            stream.write(reinterpret_cast<uint8_t*>(&header), sizeof(asset::Header));
            return offset;
        }
    } // namespace internal
} // namespace ionengine::tools::editor