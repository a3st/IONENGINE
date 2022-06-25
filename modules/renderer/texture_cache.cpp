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

ResourcePtr<GPUTexture> TextureCache::get(asset::Texture& texture) {

    {
        std::lock_guard lock(_mutex);
        uint64_t const hash = texture.hash;

        if(_data.is_valid(texture.cache_entry)) {
            return _data.get(texture.cache_entry).value;
        }
    }

    std::unique_lock lock(_mutex);


    auto result = GPUTexture::load_from_texture(*_device, texture);

    if(result.is_ok()) {
        GPUTexture gpu_texture = std::move(result.as_ok());

        auto cache_entry = CacheEntry<ResourcePtr<GPUTexture>> {
            .value = make_resource_ptr(std::move(gpu_texture)),
            .hash = texture.hash
        };

        cache_entry.value->is_wait_for_upload.store(true);

        texture.cache_entry = _data.push(std::move(cache_entry));

        return _data.get(texture.cache_entry).value;
    } else {
        throw lib::Exception(result.as_error().message);
    }
}
