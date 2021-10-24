// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class FrameBuffer {

public:

    FrameBuffer(Device* device, const FrameBufferDesc& desc);

    inline const FrameBufferDesc& GetDesc() const { return desc_; }

private:

    FrameBufferDesc desc_;
};

}