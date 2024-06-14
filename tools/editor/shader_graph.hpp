// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serializable.hpp"

namespace ionengine::tools::editor
{
    std::array<uint8_t, 8> constexpr ShaderGraphFileType{'S', 'G', 'F', 'I', 'L', 'E', '\0', '\0'};

    struct NodeData
    {
        uint64_t nodeID;
        int32_t posX;
        int32_t posY;
        uint32_t componentID;
        std::unordered_map<std::string, std::string> options;
    };

    struct ConnectionData
    {
        uint64_t connectionID;
        uint64_t source;
        uint32_t out;
        uint64_t dest;
        uint32_t in;
    };

    struct ShaderGraphData
    {
        std::vector<NodeData> nodes;
        std::vector<ConnectionData> connections;
    };

    namespace internal
    {
        auto deserialize(std::basic_istream<uint8_t>& stream) -> std::optional<ShaderGraphData>;

        auto serialize(ShaderGraphData const& object, std::basic_ostream<uint8_t>& stream) -> size_t;
    } // namespace internal
} // namespace ionengine::tools::editor

namespace ionengine::core
{
    template <>
    class Serializable<tools::editor::ShaderGraphData>
    {
      public:
        static auto deserialize(std::basic_istream<uint8_t>& stream) -> std::optional<tools::editor::ShaderGraphData>
        {
            return ionengine::tools::editor::internal::deserialize(stream);
        }

        static auto serialize(tools::editor::ShaderGraphData const& object,
                              std::basic_ostream<uint8_t>& stream) -> size_t
        {
            return ionengine::tools::editor::internal::serialize(object, stream);
        }
    };
} // namespace ionengine::core