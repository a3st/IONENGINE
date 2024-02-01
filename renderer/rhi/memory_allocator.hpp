// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine::renderer::rhi
{
    enum class MemoryAllocatorUsage
    {
        Default,
        Upload
    };

    class MemoryAllocator : public core::ref_counted_object
    {
      public:
        virtual auto get_flags() const -> MemoryAllocatorUsage = 0;
    };
} // namespace ionengine::renderer::rhi