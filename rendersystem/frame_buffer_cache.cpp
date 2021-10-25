// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "frame_buffer_cache.h"

using namespace ionengine::rendersystem;

FrameBufferCache::FrameBufferCache(lgfx::Device* device) : device_(device) {

}

lgfx::FrameBuffer* FrameBufferCache::GetFrameBuffer(const Key& key) {
    
    auto it = frame_buffers_.find(key);
    if(it != frame_buffers_.end()) {
        return it->second.get();
    } else {
        auto ret = frame_buffers_.emplace(key, std::make_unique<lgfx::FrameBuffer>(device_, key));
        return ret.first->second.get();
    }
}

void FrameBufferCache::Clear() {

    frame_buffers_.clear();
}