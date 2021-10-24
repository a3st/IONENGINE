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

lgfx::FrameBuffer* FrameBufferCache::GetFrameBuffer(const lgfx::FrameBufferDesc& desc) {
    
    auto it = frame_buffers_.find(desc);
    if(it != frame_buffers_.end()) {
        return &it->second;
    } else {
        auto result = frame_buffers_.emplace(desc, lgfx::FrameBuffer(device_, desc));
        return &result.first->second;
    }
}

void FrameBufferCache::Clear() {

    frame_buffers_.clear();
}