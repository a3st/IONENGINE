// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"

namespace ionengine
{
    class BufferAllocator : public core::ref_counted_object
    {
      public:
        BufferAllocator(core::ref_ptr<rhi::RHI> RHI, rhi::BufferUsage const usage);

        auto allocate(size_t const size) -> core::weak_ptr<rhi::Buffer>;

        auto reset() -> void;

      private:
        core::ref_ptr<rhi::RHI> RHI;
        rhi::BufferUsage usage;

        struct Bucket
        {
            std::vector<core::ref_ptr<rhi::Buffer>> elements;
            uint32_t cur;
        };

        std::map<size_t, Bucket> buckets;
    };
} // namespace ionengine