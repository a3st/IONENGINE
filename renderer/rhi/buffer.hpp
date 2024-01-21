// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "allocator.hpp"

namespace ionengine {

namespace renderer {

namespace rhi {

enum class BufferUsage {
    Vertex = 1 << 0,
    Index = 1 << 1,
    ConstantBuffer = 1 << 2,
    ShaderResource = 1 << 3,
    UnorderedAccess = 1 << 4,
    MapWrite = 1 << 5,
    MapRead = 1 << 6,
    CopySrc = 1 << 7,
    CopyDst = 1 << 8
};

DECLARE_ENUM_CLASS_BIT_FLAG(BufferUsage)

class Buffer : public core::ref_counted_object {
public:

    virtual auto get_size() -> size_t = 0;

    virtual auto get_flags() -> BufferUsageFlags = 0;
};

}

}

}