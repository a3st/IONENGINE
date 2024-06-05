// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "node.hpp"

namespace ionengine::tools::editor
{
    struct Connection : public core::ref_counted_object
    {
        Connection(uint64_t const connectionID, core::ref_ptr<Node> sourceNode, uint64_t const sourceIndex,
                   core::ref_ptr<Node> destNode, uint64_t const destIndex);

        uint64_t connectionID;
        core::ref_ptr<Node> sourceNode;
        uint64_t sourceIndex;
        core::ref_ptr<Node> destNode;
        uint64_t destIndex;
    };
} // namespace ionengine::tools::editor