// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "connection.hpp"
#include "precompiled.h"

namespace ionengine::tools::editor
{
    Connection::Connection(uint64_t const connectionID, uint64_t const sourceNode, uint64_t const sourceIndex,
                           uint64_t const destNodeID, uint64_t const destIndex)
        : connectionID(connectionID), sourceNodeID(sourceNode), sourceIndex(sourceIndex), destNodeID(destNodeID),
          destIndex(destIndex)
    {
    }

    auto Connection::getConnectionID() const -> uint64_t
    {
        return connectionID;
    }

    auto Connection::getSourceNodeID() const -> uint64_t
    {
        return sourceNodeID;
    }

    auto Connection::getSourceIndex() const -> uint64_t
    {
        return sourceIndex;
    }

    auto Connection::getDestNodeID() const -> uint64_t
    {
        return destNodeID;
    }

    auto Connection::getDestIndex() const -> uint64_t
    {
        return destIndex;
    }
} // namespace ionengine::tools::editor