// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class FrameBuffer {

public:

    FrameBuffer();
    ~FrameBuffer();
    FrameBuffer(Device* device, const FrameBufferDesc& desc);
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer(FrameBuffer&& rhs) noexcept;

    FrameBuffer& operator=(const FrameBuffer&) = delete;
    FrameBuffer& operator=(FrameBuffer&& rhs) noexcept;

    inline const FrameBufferDesc& GetDesc() const { return desc_; }

private:

    FrameBufferDesc desc_;
};

}