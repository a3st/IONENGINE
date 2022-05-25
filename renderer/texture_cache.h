// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache.h>
#include <renderer/upload_manager.h>
#include <asset/texture.h>
#include <lib/sparse_vector.h>

namespace ionengine::renderer {

class TextureCache {
public:

    TextureCache(backend::Device& device);

    TextureCache(TextureCache const&) = delete;

    TextureCache(TextureCache&& other) noexcept;

    TextureCache& operator=(TextureCache const&) = delete;

    TextureCache& operator=(TextureCache&& other) noexcept;

    ResourcePtr<GPUTexture> get(UploadManager& upload_manager, asset::Texture& texture);

private:

    backend::Device* _device;

    lib::SparseVector<CacheEntry<ResourcePtr<GPUTexture>>> _data;
};

}
