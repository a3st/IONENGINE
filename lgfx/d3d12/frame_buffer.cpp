// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "frame_buffer.h"

using namespace lgfx;

FrameBuffer::FrameBuffer() {

}

FrameBuffer::FrameBuffer(Device* device, const FrameBufferDesc& desc) : desc_(desc) {

}

FrameBuffer::FrameBuffer(FrameBuffer&& rhs) noexcept {

    std::swap(desc_, rhs.desc_);
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& rhs) noexcept {

    std::swap(desc_, rhs.desc_);
    return *this;
}