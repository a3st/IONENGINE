// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "texture_view_cache.h"

using namespace ionengine::rendersystem;

TextureViewCache::TextureViewCache() {

}

TextureViewCache::TextureViewCache(lgfx::Device* device) : device_(device) {

    rt_descriptor_pool_ = std::make_unique<lgfx::DescriptorPool>(device, lgfx::kDescriptorPoolDefaultHeapSize, lgfx::DescriptorType::kRenderTarget, lgfx::DescriptorFlags::kNone);
    ds_descriptor_pool_ = std::make_unique<lgfx::DescriptorPool>(device, lgfx::kDescriptorPoolDefaultHeapSize, lgfx::DescriptorType::kDepthStencil, lgfx::DescriptorFlags::kNone);
    sr_descriptor_pool_ = std::make_unique<lgfx::DescriptorPool>(device, lgfx::kDescriptorPoolDefaultHeapSize, lgfx::DescriptorType::kShaderResource, lgfx::DescriptorFlags::kNone);
}

TextureViewCache::TextureViewCache(TextureViewCache&& rhs) noexcept {

    std::swap(device_, rhs.device_);
    std::swap(texture_views_, rhs.texture_views_);
    std::swap(rt_descriptor_pool_, rhs.rt_descriptor_pool_);
    std::swap(ds_descriptor_pool_, rhs.ds_descriptor_pool_);
    std::swap(sr_descriptor_pool_, rhs.sr_descriptor_pool_);
}

TextureViewCache& TextureViewCache::operator=(TextureViewCache&& rhs) noexcept {

    std::swap(device_, rhs.device_);
    std::swap(texture_views_, rhs.texture_views_);
    std::swap(rt_descriptor_pool_, rhs.rt_descriptor_pool_);
    std::swap(ds_descriptor_pool_, rhs.ds_descriptor_pool_);
    std::swap(sr_descriptor_pool_, rhs.sr_descriptor_pool_);
    return *this;
}

lgfx::TextureView* TextureViewCache::GetTextureView(const Key& key) {

    auto it = texture_views_.find(key);
    if(it != texture_views_.end()) {
        return it->second.get();
    } else {
        if(key.first->GetDesc().flags & lgfx::TextureFlags::kRenderTarget) {
            auto ret = texture_views_.emplace(key, std::make_unique<lgfx::TextureView>(device_, rt_descriptor_pool_.get(), key.first, key.second));
            return ret.first->second.get();
        } else if(key.first->GetDesc().flags & lgfx::TextureFlags::kDepthStencil) {
            auto ret = texture_views_.emplace(key, std::make_unique<lgfx::TextureView>(device_, ds_descriptor_pool_.get(), key.first, key.second));
            return ret.first->second.get();
        } else {
            auto ret = texture_views_.emplace(key, std::make_unique<lgfx::TextureView>(device_, sr_descriptor_pool_.get(), key.first, key.second));
            return ret.first->second.get();
        }
    }
}

void TextureViewCache::Clear() {

    texture_views_.clear();
}