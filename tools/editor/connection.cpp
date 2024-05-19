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

    auto Connection::getConnectionID() const -> uint64_t
    {
        return connectionID;
    }

    auto Connection::getSourceNode() const -> core::ref_ptr<Node>
    {
        return sourceNode;
    }

    auto Connection::getSourceIndex() const -> uint64_t
    {
        return sourceIndex;
    }

    auto Connection::getDestNode() const -> core::ref_ptr<Node>
    {
        return destNode;
    }

    auto Connection::getDestIndex() const -> uint64_t
    {
        return destIndex;
    }
} // namespace ionengine::tools::editor