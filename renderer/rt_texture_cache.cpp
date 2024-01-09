// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/rt_texture_cache.h>

using namespace ionengine;
using namespace ionengine::renderer;

RTTextureCache::RTTextureCache(backend::Device& device) :
    _device(&device) {
}

RTTextureCache::RTTextureCache(RTTextureCache&& other) noexcept {
    _device = other._device;
    _data = std::move(other._data);
}

RTTextureCache& RTTextureCache::operator=(RTTextureCache&& other) noexcept {
    _device = other._device;
    _data = std::move(other._data);
    return *this;
}

ResourcePtr<GPUTexture> RTTextureCache::get(asset::Texture& texture) {
        
    std::unique_lock lock(_mutex);

    uint64_t const hash = texture.hash;

    auto it = _data.find(hash);

    if(it != _data.end()) {

        return it->second.value;

    } else {

        auto result = GPUTexture::load_from_texture(*_device, texture);

        if(result.is_ok()) {

            auto cache_entry = CacheEntry<ResourcePtr<GPUTexture>> {
                .value = make_resource_ptr(result.as_ok()),
                .hash = hash
            };
            
            auto inserted = _data.insert({ hash, std::move(cache_entry) });
            return inserted.first->second.value;

        } else {
            throw lib::Exception(result.as_error().message);
        }
    }
}
