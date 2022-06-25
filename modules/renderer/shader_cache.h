// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache.h>
#include <renderer/resource_ptr.h>
#include <renderer/gpu_program.h>
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

    ResourcePtr<GPUProgram> get(asset::ShaderVariant& shader_variant);

private:

    backend::Device* _device;
    std::shared_mutex _mutex;
    lib::SparseVector<CacheEntry<ResourcePtr<GPUProgram>>> _data;
};

}
