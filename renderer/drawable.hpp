// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "buffer.hpp"

namespace ionengine {

namespace renderer {

class Backend;

class Drawable : public core::ref_counted_object {
public:

    Drawable(
        Backend& backend,
        BufferAllocator<LinearAllocator>& drawable_allocator
    );

protected:

    BufferAllocation vertices;
    BufferAllocation indices;
};

}

}