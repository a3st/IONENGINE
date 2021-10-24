// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "frame_buffer_cache.h"

using namespace ionengine::rendersystem;

FrameBufferCache::FrameBufferCache(lgfx::Device* device) : device_(device) {

}

lgfx::FrameBuffer* FrameBufferCache::GetFrameBuffer(const lgfx::FrameBufferDesc& desc) {
    
    auto it = frame_buffers_.find(desc);
    if(it != frame_buffers_.end()) {
        return it->second.get();
    } else {
        auto result = frame_buffers_.emplace(desc, std::make_unique<lgfx::FrameBuffer>(device_, desc));
        return result.first->second.get();
    }
}

void FrameBufferCache::Clear() {

    frame_buffers_.clear();
}