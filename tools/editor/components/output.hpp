// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../node.hpp"

namespace ionengine::tools::editor
{
    class OutputNode : public Node
    {
      public:
        OutputNode(uint64_t const nodeID, uint32_t const posX, uint32_t const posY)
            : Node(nodeID, "Output Image", posX, posY, {{"Color", "float4"}}, {})
        {
        }
    };
} // namespace ionengine::tools::editor