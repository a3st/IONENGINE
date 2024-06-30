// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graph/node.hpp"

namespace ionengine::tools::editor
{
    IONENGINE_NODE_COMPONENT_BEGIN(Input, "Input Node", false, std::nullopt, true)
    auto generateResourceShaderCode(Node const& node) const -> std::string override
    {
        std::stringstream stream;
        stream << "struct MaterialData {\n";

        bool isFirst = true;
        for (auto const& output : node.outputs)
        {
            if (!isFirst)
            {
                stream << "\n";
            }
            stream << "\t" << output.socketType << " " << output.socketName << ";";
            isFirst = false;
        }

        stream << "\n};\n\n"
               << "[[fx::shader_constant]] MaterialData materialData;";
        return stream.str();
    }

    auto generateComputeShaderCode(Node const& node) const -> std::string override
    {
        return "materialData.##__CONNECTION__##";
    }
    IONENGINE_NODE_COMPONENT_END
} // namespace ionengine::tools::editor