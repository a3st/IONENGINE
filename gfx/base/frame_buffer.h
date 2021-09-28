// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class FrameBuffer {
public:
    virtual ~FrameBuffer() = default;
    virtual const FrameBufferDesc& get_desc() const = 0;
};

}