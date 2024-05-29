// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../node.hpp"

namespace ionengine::tools::editor
{
    class InputNode : public Node
    {
      public:
        InputNode(uint64_t const nodeID, uint32_t const posX, uint32_t const posY,
                  std::vector<NodeSocketInfo> const& outputs)
            : Node(nodeID, "Input Node", posX, posY, outputs, {}, true)
        {
        }

        auto generateInitialShaderCode() -> std::string override
        {
            return "";
        }

        auto generateResourceShaderCode() -> std::string override
        {
            std::stringstream stream;
            stream << "struct MaterialData {" << std::endl;

            bool isFirst = true;
            for (auto const& output : this->getOutputSockets())
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

        auto generateComputeShaderCode() -> std::string override
        {
            return "materialData.##__CONNECTION__##";
        }
    };
} // namespace ionengine::tools::editor