// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../../precompiled.h"
#include "buffer_view.h"

using namespace ionengine::rendersystem;

BufferViewCache::BufferViewCache(lgfx::Device* device) : 
    device_(device),
    sr_descriptor_pool_(device, lgfx::kDescriptorPoolDefaultHeapSize, lgfx::DescriptorType::kShaderResource, lgfx::DescriptorFlags::kNone) {

}

lgfx::BufferView* BufferViewCache::GetBufferView(lgfx::Buffer* buffer, const lgfx::Format index_format, const uint32_t stride) {

    BufferViewCache::Key kBufferViewKey{};
    kBufferViewKey.buffer = buffer;
    kBufferViewKey.index_format = index_format;
    kBufferViewKey.stride = stride;

    auto it = buffer_views_.find(kBufferViewKey);
    if(it != buffer_views_.end()) {
        return it->second.get();
    } else {
        if(kBufferViewKey.buffer->GetFlags() & lgfx::BufferFlags::kIndexBuffer | kBufferViewKey.buffer->GetFlags() & lgfx::BufferFlags::kVertexBuffer) {
            auto ret = buffer_views_.emplace(kBufferViewKey, std::make_unique<lgfx::BufferView>(device_, nullptr, kBufferViewKey.buffer, kBufferViewKey.index_format, kBufferViewKey.stride));
            return ret.first->second.get();
        } else {
            auto ret = buffer_views_.emplace(kBufferViewKey, std::make_unique<lgfx::BufferView>(device_, &sr_descriptor_pool_, kBufferViewKey.buffer, lgfx::Format::kUnknown, 0));
            return ret.first->second.get();
        }
    }
}