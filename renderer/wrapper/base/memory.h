// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class Memory {
public:

    virtual ~Memory() = default;
    virtual MemoryType get_type() const = 0;
    virtual BufferFlags get_flags() const = 0;
};


}