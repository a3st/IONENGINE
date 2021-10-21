

#include "../precompiled.h"
#include "frame_buffer_cache.h"

using namespace ionengine::rendersystem;

FrameBufferCache::FrameBufferCache() {

}

FrameBufferCache::FrameBufferCache(lgfx::Device* device) {

}

FrameBufferCache::FrameBufferCache(FrameBufferCache&& rhs) noexcept {

}

FrameBufferCache& FrameBufferCache::operator=(FrameBufferCache&& rhs) noexcept {

    return *this;
}

lgfx::FrameBuffer* FrameBufferCache::GetFrameBuffer(const Key& key) {

    return nullptr;
}

void FrameBufferCache::Clear() {

    frame_buffers_.clear();
}