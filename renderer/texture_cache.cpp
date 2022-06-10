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

ResourcePtr<GPUTexture> TextureCache::get(UploadManager& upload_manager, asset::Texture& texture) {

    std::lock_guard lock(_mutex);

    uint64_t const hash = texture.hash;

    if(_data.is_valid(texture.cache_entry)) {

        auto& cache_entry = _data.get(texture.cache_entry);

        if(cache_entry.value->is_wait_for_upload.load()) {
            upload_manager.upload_texture_data(cache_entry.value, std::span<uint8_t const>(texture.data.data(), texture.data.size()));
        }

        return cache_entry.value;

    } else {

        auto result = GPUTexture::load_from_texture(*_device, texture);

        if(result.is_ok()) {

            auto cache_entry = CacheEntry<ResourcePtr<GPUTexture>> {
                .value = make_resource_ptr(result.value()),
                .hash = hash
            };

            cache_entry.value->is_wait_for_upload.store(true);

            upload_manager.upload_texture_data(cache_entry.value, std::span<uint8_t const>(texture.data.data(), texture.data.size()));
            texture.cache_entry = _data.push(std::move(cache_entry));

        } else {
            throw lib::Exception(result.error_value().message);
        }
        
        auto& cache_entry = _data.get(texture.cache_entry);
        return cache_entry.value;
    }
}
