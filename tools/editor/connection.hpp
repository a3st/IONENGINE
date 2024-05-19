// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "node.hpp"

namespace ionengine::tools::editor
{
    class Connection : public core::ref_counted_object
    {
      public:
        Connection(uint64_t const connectionID, core::ref_ptr<Node> sourceNode, uint64_t const sourceIndex,
                   core::ref_ptr<Node> destNode, uint64_t const destIndex);

        auto getConnectionID() const -> uint64_t;

        auto getSourceNode() const -> core::ref_ptr<Node>;

        auto getSourceIndex() const -> uint64_t;

        auto getDestNode() const -> core::ref_ptr<Node>;

        auto getDestIndex() const -> uint64_t;

      private:
        uint64_t connectionID;
        core::ref_ptr<Node> sourceNode;
        uint64_t sourceIndex;
        core::ref_ptr<Node> destNode;
        uint64_t destIndex;
    };
} // namespace ionengine::tools::editor