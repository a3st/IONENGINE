// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class Buffer {
public:

    virtual ~Buffer() = default;

    virtual void bind_memory(Memory* memory, const uint64 offset) = 0;

    virtual BufferType get_type() const = 0;

    virtual byte* map() = 0;

    virtual void unmap() = 0;
};

}