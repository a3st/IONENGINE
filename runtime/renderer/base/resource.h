// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Resource {
public:

    virtual ~Resource() = default;
    virtual void bind_memory(Memory& memory, const uint64 offset) = 0;

    virtual Format get_format() const = 0;
    virtual uint64 get_width() const = 0;
    virtual uint32 get_height() const = 0;
};

}