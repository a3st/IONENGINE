// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/texture_cache.h>

using namespace ionengine;
using namespace ionengine::renderer;

TextureCache::TextureCache(backend::Device& device) :
    _device(&device) {
}

TextureCache::TextureCache(TextureCache&& other) noexcept {
    _device = other._device;
    _data = std::move(other._data);
}

TextureCache& TextureCache::operator=(TextureCache&& other) noexcept {
    _device = other._device;
    _data = std::move(other._data);
    return *this;
}

std::shared_ptr<GPUTexture> TextureCache::get(UploadContext& context, asset::Texture& texture) {
        
    uint64_t const total_hash = texture.hash();

    if(_data.is_valid(texture.cache_entry())) {
        auto& cache_entry = _data.get(texture.cache_entry());
            
        if(cache_entry.hash != texture.cache_entry()) {

        }

        return cache_entry.value;

    } else {

        {
            auto gpu_texture = GPUTexture::sampler(*_device, context, texture);

            auto cache_entry = CacheEntry<std::shared_ptr<GPUTexture>> {
                .value = gpu_texture,
                .hash = total_hash
            };

            texture.cache_entry(_data.push(std::move(cache_entry)));
        }
        auto& cache_entry = _data.get(texture.cache_entry());
        return cache_entry.value;
    }
}
