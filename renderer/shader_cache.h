// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache.h>
#include <renderer/resource_ptr.h>
#include <renderer/shader.h>
#include <asset/material.h>
#include <lib/sparse_vector.h>

namespace ionengine::renderer {

class ShaderCache {
public:

    ShaderCache(backend::Device& device);

    ShaderCache(ShaderCache const&) = delete;

    ShaderCache(ShaderCache&& other) noexcept;

    ShaderCache& operator=(ShaderCache const&) = delete;

    ShaderCache& operator=(ShaderCache&& other) noexcept;

    void cache_shader(std::filesystem::path const shader_path);

    ResourcePtr<Shader> get(asset::AssetPtr<asset::Material> material, std::string_view const pass_name);

    void update(float const delta_time);

private:

    backend::Device* _device;

    std::shared_mutex _mutex;

    std::unordered_map<std::string, size_t> _pass_hashes;
    lib::SparseVector<CacheEntry<ResourcePtr<Shader>>> _data;
};

}
