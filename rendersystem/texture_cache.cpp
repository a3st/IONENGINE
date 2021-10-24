// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "texture_cache.h"

using namespace ionengine::rendersystem;

TextureCache::TextureCache() {

}

TextureCache::TextureCache(lgfx::Device* device) : device_(device) {

    rt_memory_pool_ = lgfx::MemoryPool(device, lgfx::kMemoryPoolDefaultHeapSize, 0, lgfx::MemoryType::kDefault, lgfx::MemoryFlags::kRT_DS);
    sr_memory_pool_ = lgfx::MemoryPool(device, lgfx::kMemoryPoolDefaultHeapSize, 0, lgfx::MemoryType::kDefault, lgfx::MemoryFlags::kNon_RT_DS);
}

TextureCache::TextureCache(TextureCache&& rhs) noexcept {

    std::swap(device_, rhs.device_);
    std::swap(textures_, rhs.textures_);
    std::swap(rt_memory_pool_, rhs.rt_memory_pool_);
    std::swap(sr_memory_pool_, rhs.sr_memory_pool_);
}

TextureCache& TextureCache::operator=(TextureCache&& rhs) noexcept {

    std::swap(device_, rhs.device_);
    std::swap(textures_, rhs.textures_);
    std::swap(rt_memory_pool_, rhs.rt_memory_pool_);
    std::swap(sr_memory_pool_, rhs.sr_memory_pool_);
    return *this;
}

lgfx::Texture* TextureCache::GetTexture(const lgfx::TextureDesc& desc) {

    auto it = textures_.find(desc);
    if(it != textures_.end()) {
        if(it->second.entry_index > static_cast<uint32_t>(it->second.textures.size())) {
            if(desc.flags & lgfx::TextureFlags::kRenderTarget || desc.flags & lgfx::TextureFlags::kDepthStencil) {
                auto& ret = it->second.textures.emplace_back(device_, &rt_memory_pool_, desc);
                ++it->second.entry_index;
                return &ret;
            } else {
                auto& ret = it->second.textures.emplace_back(device_, &sr_memory_pool_, desc);
                ++it->second.entry_index;
                return &ret;
            }
        } else {
            auto& ret = it->second.textures[it->second.entry_index];
            ++it->second.entry_index;
            return &ret;
        }
    } else {
        auto& result = textures_.emplace(desc, TextureEntry { 0 });
        if(desc.flags & lgfx::TextureFlags::kRenderTarget || desc.flags & lgfx::TextureFlags::kDepthStencil) {
            auto& ret = result.first->second.textures.emplace_back(device_, &rt_memory_pool_, desc);
            ++result.first->second.entry_index;
            return &ret;
        } else {
            auto& ret = result.first->second.textures.emplace_back(device_, &sr_memory_pool_, desc);
            ++result.first->second.entry_index;
            return &ret;
        }
    }
}

void TextureCache::Clear() {

    for(auto& it = textures_.begin(); it != textures_.end(); ++it) {
        it->second.entry_index = 0;
    }
}