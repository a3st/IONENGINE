// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "texture_cache.h"

using namespace ionengine::rendersystem;

TextureCache::TextureCache(lgfx::Device* device) : device_(device) {

    rtds_memory_pool_ = std::make_unique<lgfx::MemoryPool>(device, lgfx::kMemoryPoolDefaultHeapSize, 0, lgfx::MemoryType::kDefault, lgfx::MemoryFlags::kRT_DS);
    sr_memory_pool_ = std::make_unique<lgfx::MemoryPool>(device, lgfx::kMemoryPoolDefaultHeapSize, 0, lgfx::MemoryType::kDefault, lgfx::MemoryFlags::kNon_RT_DS);
}

lgfx::Texture* TextureCache::GetTexture(const Key& key) {

    auto it = textures_.find(key);
    if(it != textures_.end()) {
        if(it->second.entry_index >= static_cast<uint32_t>(it->second.textures.size())) {
            if(key.flags & lgfx::TextureFlags::kRenderTarget || key.flags & lgfx::TextureFlags::kDepthStencil) {
                auto& ret = it->second.textures.emplace_back(std::make_unique<lgfx::Texture>(device_, rtds_memory_pool_.get(), key));
                ++it->second.entry_index;
                return ret.get();
            } else {
                auto& ret = it->second.textures.emplace_back(std::make_unique<lgfx::Texture>(device_, sr_memory_pool_.get(), key));
                ++it->second.entry_index;
                return ret.get();
            }
        } else {
            auto& ret = it->second.textures[it->second.entry_index];
            ++it->second.entry_index;
            return ret.get();
        }
    } else {
        auto result = textures_.emplace(key, TextureEntry { });
        if(key.flags & lgfx::TextureFlags::kRenderTarget || key.flags & lgfx::TextureFlags::kDepthStencil) {
            auto& ret = result.first->second.textures.emplace_back(std::make_unique<lgfx::Texture>(device_, rtds_memory_pool_.get(), key));
            ++result.first->second.entry_index;
            return ret.get();
        } else {
            auto& ret = result.first->second.textures.emplace_back(std::make_unique<lgfx::Texture>(device_, sr_memory_pool_.get(), key));
            ++result.first->second.entry_index;
            return ret.get();
        }
    }
}

void TextureCache::Clear() {

    for(auto it = textures_.begin(); it != textures_.end(); ++it) {
        it->second.entry_index = 0;
    }
}