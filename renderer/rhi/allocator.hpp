// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine {

namespace renderer {

namespace rhi {

class MemoryHeap : public core::ref_counted_object {
    
};

struct MemoryAllocation {
    core::ref_ptr<MemoryHeap> heap{nullptr};
    uint64_t offset;
    size_t size;
};

class MemoryAllocator : public core::ref_counted_object {
public:

    virtual auto allocate(size_t const size) -> MemoryAllocation = 0;

    virtual auto deallocate(MemoryAllocation const& allocation) -> void = 0;

    virtual auto reset() -> void = 0;
};

}

}

}