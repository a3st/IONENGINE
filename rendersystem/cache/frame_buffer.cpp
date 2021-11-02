// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../../precompiled.h"
#include "frame_buffer.h"

using namespace ionengine::rendersystem;

FrameBufferCache::FrameBufferCache(lgfx::Device* device) : device_(device) {

}

lgfx::FrameBuffer* FrameBufferCache::GetFrameBuffer(lgfx::RenderPass* render_pass, const uint32_t width, const uint32_t height, const std::span<lgfx::TextureView* const> colors, lgfx::TextureView* depth_stencil) {
    
    static FrameBufferCache::Key kFrameBufferKey{};
    kFrameBufferKey.render_pass = render_pass;
    kFrameBufferKey.width = width;
    kFrameBufferKey.height = height;
    kFrameBufferKey.colors.resize(colors.size());
    std::copy(colors.begin(), colors.end(), kFrameBufferKey.colors.begin());
    kFrameBufferKey.depth_stencil = depth_stencil;

    auto it = frame_buffers_.find(kFrameBufferKey);
    if(it != frame_buffers_.end()) {
        return it->second.get();
    } else {
        auto ret = frame_buffers_.emplace(kFrameBufferKey, std::make_unique<lgfx::FrameBuffer>(device_, 
            kFrameBufferKey.render_pass, kFrameBufferKey.width, kFrameBufferKey.height, kFrameBufferKey.colors, kFrameBufferKey.depth_stencil));
        return ret.first->second.get();
    }
}