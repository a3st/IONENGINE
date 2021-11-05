// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "frame_buffer.h"

using namespace lgfx;

FrameBuffer::FrameBuffer(
    Device* const device, 
    RenderPass* const render_pass,
    const uint32_t width, const uint32_t height,
    const std::span<TextureView* const> colors,
    TextureView* const depth_stencil) : 
        render_pass_(render_pass), 
        width_(width), height_(height), 
        colors_(colors.begin(), colors.end()),
        depth_stencil_(depth_stencil) {

}

FrameBuffer::FrameBuffer(FrameBuffer&& rhs) noexcept {

    std::swap(render_pass_, rhs.render_pass_);
    std::swap(width_, rhs.width_);
    std::swap(height_, rhs.height_);
    std::swap(colors_, rhs.colors_);
    std::swap(depth_stencil_, rhs.depth_stencil_);
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& rhs) noexcept {

    std::swap(render_pass_, rhs.render_pass_);
    std::swap(width_, rhs.width_);
    std::swap(height_, rhs.height_);
    std::swap(colors_, rhs.colors_);
    std::swap(depth_stencil_, rhs.depth_stencil_);
    return *this;
}