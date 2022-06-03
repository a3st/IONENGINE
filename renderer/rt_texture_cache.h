// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache.h>
#include <renderer/resource_ptr.h>
#include <renderer/gpu_texture.h>
#include <asset/texture.h>

namespace ionengine::renderer {

class RTTextureCache {
public:

    RTTextureCache(backend::Device& device);

    RTTextureCache(RTTextureCache const&) = delete;

    RTTextureCache(RTTextureCache&& other) noexcept;

    RTTextureCache& operator=(RTTextureCache const&) = delete;

    RTTextureCache& operator=(RTTextureCache&& other) noexcept;

    ResourcePtr<GPUTexture> get(asset::Texture& texture);

private:

    backend::Device* _device;

    std::unordered_map<uint64_t, CacheEntry<ResourcePtr<GPUTexture>>> _data;
};

}
