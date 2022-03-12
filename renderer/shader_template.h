// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>

namespace ionengine::renderer {

enum class ShaderDomain {
    Surface,
    Decal,
    PostProcess
};

enum class ShaderBlendMode {
    Opaque,
    Transparent
};

struct ShaderTemplate {
    
    struct PassData {
        std::span<Handle<Shader> const> shaders;
        std::unordered_map<uint32_t, uint32_t> descriptor_indices;    
    };

    std::vector<Handle<Shader>> shaders;
    std::unordered_map<uint32_t, PassData> pass_data;
};

}