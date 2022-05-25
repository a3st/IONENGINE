// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache.h>
#include <renderer/resource_ptr.h>
#include <renderer/shader_program.h>
#include <asset/technique.h>
#include <lib/sparse_vector.h>

namespace ionengine::renderer {

class ShaderCache {
public:

    ShaderCache(backend::Device& device);

    ShaderCache(ShaderCache const&) = delete;

    ShaderCache(ShaderCache&& other) noexcept;

    ShaderCache& operator=(ShaderCache const&) = delete;

    ShaderCache& operator=(ShaderCache&& other) noexcept;

    ResourcePtr<ShaderProgram> get(asset::Technique& technique);

    void update(float const delta_time);

private:

    backend::Device* _device;

    lib::SparseVector<CacheEntry<ResourcePtr<ShaderProgram>>> _data;
};

}
