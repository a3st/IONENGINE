// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class FrameBuffer {

friend class CommandBuffer;

public:

    FrameBuffer(
        Device* device, 
        RenderPass* render_pass,
        const uint32_t width, const uint32_t height,
        const std::span<TextureView*> colors,
        TextureView* depth_stencil);

    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer(FrameBuffer&&) = delete;

    FrameBuffer& operator=(const FrameBuffer&) = delete;
    FrameBuffer& operator=(FrameBuffer&&) = delete;

private:

    RenderPass* render_pass_;
    uint32_t width_;
    uint32_t height_;
    std::vector<TextureView*> colors_;
    TextureView* depth_stencil_;
};

}