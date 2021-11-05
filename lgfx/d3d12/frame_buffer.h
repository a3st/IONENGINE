// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class FrameBuffer {

friend class CommandBuffer;

public:

    FrameBuffer(
        Device* const device, 
        RenderPass* const render_pass,
        const uint32_t width, const uint32_t height,
        const std::span<TextureView* const> colors,
        TextureView* const depth_stencil);

    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer(FrameBuffer&& rhs) noexcept;

    FrameBuffer& operator=(const FrameBuffer&) = delete;
    FrameBuffer& operator=(FrameBuffer&& rhs) noexcept;

private:

    RenderPass* render_pass_;
    uint32_t width_;
    uint32_t height_;
    std::vector<TextureView*> colors_;
    TextureView* depth_stencil_;
};

}