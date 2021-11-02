// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "frame_buffer.h"

using namespace lgfx;

FrameBuffer::FrameBuffer(
    Device* device, 
    RenderPass* render_pass,
    const uint32_t width, const uint32_t height,
    const std::span<TextureView* const> colors,
    TextureView* depth_stencil) : 
        render_pass_(render_pass), 
        width_(width), height_(height), 
        colors_(colors.begin(), colors.end()),
        depth_stencil_(depth_stencil) {

}