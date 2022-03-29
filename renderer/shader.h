// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <lib/math/vector.h>

namespace ionengine::renderer {

template<class Type>
struct ShaderBindingType {
    uint32_t index;
};

template<>
struct ShaderBindingType<backend::Sampler> {
    uint32_t index;
};

template<>
struct ShaderBindingType<backend::Buffer> {
    uint32_t index;
};

template<>
struct ShaderBindingType<backend::Texture> {
    uint32_t index;
};

using ShaderBinding = std::variant<ShaderBindingType<backend::Sampler>, ShaderBindingType<backend::Buffer>, ShaderBindingType<backend::Texture>>;

struct ShaderSetCreateInfo {

    std::vector<Handle<backend::Shader>> shaders;
    std::unordered_map<uint32_t, ShaderBinding> bindings;

    ShaderSetCreateInfo& set_shader(Handle<backend::Shader> const& shader) {

        shaders.emplace_back(shader);
        return *this;
    }

    ShaderSetCreateInfo& set_binding(uint32_t const id, ShaderBinding const& shader_binding) {

        bindings[id] = shader_binding;
        return *this;
    }
};

class ShaderSet {
public:

    ShaderSet(backend::Backend& backend, ShaderSetCreateInfo const& create_info);

    std::span<Handle<backend::Shader> const> shaders() const;

    ShaderBinding const& bindings(uint32_t const id) const;

private:

    friend class ShaderSetBinder;

    std::unordered_map<uint32_t, ShaderBinding> _bindings;
    std::vector<Handle<backend::Shader>> _shaders;
};

/*
class ShaderEffectBinder {
public:

    ShaderEffectBinder(ShaderEffect& shader_effect);

    ShaderEffectBinder& bind(ShaderBindingId const id, std::variant<Handle<backend::Texture>, Handle<backend::Buffer>, Handle<backend::Sampler>> const& target);

    void update(backend::Backend& backend, Handle<backend::DescriptorSet> const& descriptor_set);

private:

    ShaderEffect* _shader_effect;

    std::array<backend::DescriptorWriteDesc, 64> _descriptor_updates;
    uint32_t _update_count{0};
};
*/

struct StateParameters {
    backend::CullMode cull_mode;
    backend::FillMode fill_mode;
};

struct ShaderData {

    struct PassData {
        StateParameters parameters;
        ShaderSet set;
    };

    std::unordered_map<uint32_t, ShaderBinding*> bindings;
    std::vector<PassData> passes;
};
    
}
