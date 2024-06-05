// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "connection.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    Connection::Connection(uint64_t const connectionID, core::ref_ptr<Node> sourceNode, uint64_t const sourceIndex,
                           core::ref_ptr<Node> destNode, uint64_t const destIndex)
        : connectionID(connectionID), sourceNode(sourceNode), sourceIndex(sourceIndex), destNode(destNode),
          destIndex(destIndex)
    {
    }
} // namespace ionengine::tools::editor