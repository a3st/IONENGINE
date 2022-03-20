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

    using ConstBufferInfo = std::pair<uint32_t, uint32_t>;
    using SamplerInfo = uint32_t;
    using BindingInfo = std::variant<ConstBufferInfo, SamplerInfo>;
    
    struct PassData {
        std::span<Handle<Shader> const> shaders;
        std::unordered_map<uint32_t, BindingInfo> binding_indices;
    };

    std::vector<Handle<Shader>> shaders;
    
    std::unordered_map<uint32_t, PassData> low_pass_data;
    std::unordered_map<uint32_t, PassData> high_pass_data;
};

}
