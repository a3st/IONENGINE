// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../../precompiled.h"
#include "buffer.h"

using namespace ionengine::rendersystem;

BufferCache::BufferCache(lgfx::Device* device) : 
    device_(device),
    memory_pool_(device, 1024 * 1024, 0, lgfx::MemoryType::kDefault, lgfx::MemoryFlags::kBuffers) {

}

lgfx::Buffer* BufferCache::GetBuffer(const size_t size, const lgfx::BufferFlags flags) {

    static BufferCache::Key kBufferKey{};
    kBufferKey.size = size;
    kBufferKey.flags = flags;

    auto it = buffers_.find(kBufferKey);
    if(it != buffers_.end()) {
        if(it->second.entry_index >= static_cast<uint32_t>(it->second.buffers.size())) {
            auto& ret = it->second.buffers.emplace_back(std::make_unique<lgfx::Buffer>(device_, &memory_pool_, 
                kBufferKey.size, kBufferKey.flags));
            ++it->second.entry_index;
            return ret.get();
        } else {
            auto& ret = it->second.buffers[it->second.entry_index];
            ++it->second.entry_index;
            return ret.get();
        }
    } else {
        auto result = buffers_.emplace(kBufferKey, BufferEntry { });
        auto& ret = result.first->second.buffers.emplace_back(std::make_unique<lgfx::Buffer>(device_, &memory_pool_, 
            kBufferKey.size, kBufferKey.flags));
        ++result.first->second.entry_index;
        return ret.get();
    }
}

void BufferCache::Clear() {

    for(auto it = buffers_.begin(); it != buffers_.end(); ++it) {
        it->second.entry_index = 0;
    }
}