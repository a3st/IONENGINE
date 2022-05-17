// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache.h>
#include <renderer/gpu_texture.h>
#include <renderer/upload_context.h>
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

    std::shared_ptr<GPUTexture> get(UploadContext& context, asset::Texture& texture);

private:

    backend::Device* _device;

    lib::SparseVector<CacheEntry<std::shared_ptr<GPUTexture>>> _data;
};

}
