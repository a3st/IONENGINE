// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../node.hpp"

namespace ionengine::tools::editor
{
    IONENGINE_NODE_COMPONENT_BEGIN(Input, "Input Node", false, std::nullopt, true)
    auto generateInitialShaderCode(Node const& node) -> std::string override
    {
        return "";
    }

    auto generateResourceShaderCode(Node const& node) -> std::string override
    {
        std::stringstream stream;
        stream << "struct MaterialData {" << std::endl;

        bool isFirst = true;
        for (auto const& output : node.outputs)
        {
            if (!isFirst)
            {
                stream << std::endl;
            }
            stream << "\t" << output.socketType << " " << output.socketName << ";";
            isFirst = false;
        }

        stream << "};" << std::endl
               << std::endl
               << "[fx::shader_constant]" << std::endl
               << "MaterialData materialData;" << std::endl;
        return stream.str();
    }

    auto generateComputeShaderCode(Node const& node) -> std::string override
    {
        return "materialData.##__CONNECTION__##";
    }
    IONENGINE_NODE_COMPONENT_END
} // namespace ionengine::tools::editor