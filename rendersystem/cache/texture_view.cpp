// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../../precompiled.h"
#include "texture_view.h"

using namespace ionengine::rendersystem;

TextureViewCache::TextureViewCache(lgfx::Device* device) : 
    device_(device),
    rt_descriptor_pool_(device, lgfx::kDescriptorPoolDefaultHeapSize, lgfx::DescriptorType::kRenderTarget, lgfx::DescriptorFlags::kNone),
    ds_descriptor_pool_(device, lgfx::kDescriptorPoolDefaultHeapSize, lgfx::DescriptorType::kDepthStencil, lgfx::DescriptorFlags::kNone),
    sr_descriptor_pool_(device, lgfx::kDescriptorPoolDefaultHeapSize, lgfx::DescriptorType::kShaderResource, lgfx::DescriptorFlags::kNone) {

}

lgfx::TextureView* TextureViewCache::GetTextureView(const Key& key) {

    auto it = texture_views_.find(key);
    if(it != texture_views_.end()) {
        return it->second.get();
    } else {
        if(key.texture->GetFlags() & lgfx::TextureFlags::kRenderTarget) {
            auto ret = texture_views_.emplace(key, std::make_unique<lgfx::TextureView>(device_, &rt_descriptor_pool_, key.texture, 
                key.dimension, key.base_mip_level, key.base_array_layer, key.array_layer_count));
            return ret.first->second.get();
        } else if(key.texture->GetFlags() & lgfx::TextureFlags::kDepthStencil) {
            auto ret = texture_views_.emplace(key, std::make_unique<lgfx::TextureView>(device_, &ds_descriptor_pool_, key.texture, 
                key.dimension, key.base_mip_level, key.base_array_layer, key.array_layer_count));
            return ret.first->second.get();
        } else {
            auto ret = texture_views_.emplace(key, std::make_unique<lgfx::TextureView>(device_, &sr_descriptor_pool_, key.texture, 
                key.dimension, key.base_mip_level, key.base_array_layer, key.array_layer_count));
            return ret.first->second.get();
        }
    }
}