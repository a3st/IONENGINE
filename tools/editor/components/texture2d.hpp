// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../node.hpp"

namespace ionengine::tools::editor
{
    class Texture2DNode : public Node
    {
      public:
        Texture2DNode(uint64_t const nodeID, uint32_t const posX, uint32_t const posY)
            : Node(nodeID, "Texture2D", posX, posY, {}, {{"Texture", "Texture2D"}}, true)
        {
        }

        auto generateInitialShaderCode() -> std::string override
        {
            return "";
        }

        auto generateResourceShaderCode() -> std::string override
        {
            return R"(
                [fx::shader_constant]
                Texture2D ##textureName##Tex;
            )";
        }

        auto generateComputeShaderCode() -> std::string override
        {
            return "##textureName##Tex";
        }
    };
} // namespace ionengine::tools::editor