// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache/cache.h>
#include <renderer/frontend/shader_program.h>
#include <lib/sparse_vector.h>

namespace ionengine::renderer::cache {

class ShaderCache {
public:

    ShaderCache() = default;

    frontend::ShaderProgram& get(frontend::Context& context, asset::Technique& technique);

private:

    lib::SparseVector<CacheEntry<frontend::ShaderProgram>> _shaders;
};

}