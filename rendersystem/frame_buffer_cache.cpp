// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "frame_buffer_cache.h"

using namespace ionengine::rendersystem;

FrameBufferCache::FrameBufferCache() {

}

FrameBufferCache::FrameBufferCache(lgfx::Device* device) : device_(device) {

}

FrameBufferCache::FrameBufferCache(FrameBufferCache&& rhs) noexcept {

    std::swap(device_, rhs.device_);
    std::swap(frame_buffers_, rhs.frame_buffers_);
}

FrameBufferCache& FrameBufferCache::operator=(FrameBufferCache&& rhs) noexcept {

    std::swap(device_, rhs.device_);
    std::swap(frame_buffers_, rhs.frame_buffers_);
    return *this;
}

lgfx::FrameBuffer* FrameBufferCache::GetFrameBuffer(const Key& key) {

    auto it = frame_buffers_.find(key);
    if(it != frame_buffers_.end()) {
        return &it->second;
    } else {
        lgfx::FrameBufferDesc desc{};
        desc.render_pass = key.render_pass;
        desc.width = key.width;
        desc.height = key.height;
        desc.colors = key.colors;
        desc.depth_stencil = key.depth_stencil;

        auto result = frame_buffers_.emplace(key, lgfx::FrameBuffer(device_, desc));
        return &result.first->second;
    }
}

void FrameBufferCache::Clear() {

    frame_buffers_.clear();
}