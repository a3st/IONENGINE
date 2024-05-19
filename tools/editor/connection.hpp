// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine::tools::editor
{
    class Connection : public core::ref_counted_object
    {
      public:
        Connection(uint64_t const connectionID, uint64_t const sourceNode, uint64_t const sourceIndex,
                   uint64_t const destNodeID, uint64_t const destIndex);

        auto getConnectionID() const -> uint64_t;

        auto getSourceNodeID() const -> uint64_t;

        auto getSourceIndex() const -> uint64_t;

        auto getDestNodeID() const -> uint64_t;

        auto getDestIndex() const -> uint64_t;

      private:
        uint64_t connectionID;
        uint64_t sourceNodeID;
        uint64_t sourceIndex;
        uint64_t destNodeID;
        uint64_t destIndex;
    };
} // namespace ionengine::tools::editor