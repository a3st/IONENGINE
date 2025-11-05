// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"

namespace ionengine
{
    class BufferPool : public core::ref_counted_object
    {
      public:
        BufferPool(core::ref_ptr<rhi::RHI> rhi, rhi::BufferUsage const bufferUsage);

        auto allocate(size_t const bufferSize) -> core::weak_ptr<rhi::Buffer>;

        auto reset() -> void;

      private:
        core::ref_ptr<rhi::RHI> _rhi;
        rhi::BufferUsage _bufferUsage;

        struct Bucket
        {
            std::vector<core::ref_ptr<rhi::Buffer>> buffers;
            uint32_t current;
        };

        std::map<size_t, Bucket> _buckets;
    };
} // namespace ionengine