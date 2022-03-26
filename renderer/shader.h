// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/data.h>
#include <lib/math/vector.h>

namespace ionengine::renderer {

template<class Type>
struct ShaderBinding {
    uint32_t index;
    std::u8string name;
};

template<>
struct ShaderBinding<backend::Sampler> {
    uint32_t index;
    std::u8string name;
};

template<>
struct ShaderBinding<backend::Buffer> {
    uint32_t index;
    std::u8string name;
};

template<>
struct ShaderBinding<backend::Texture> {
    uint32_t index;
    std::u8string name;
};

using ShaderBindingDesc = std::variant<ShaderBinding<backend::Sampler>, ShaderBinding<backend::Buffer>,ShaderBinding<backend::Texture>>;

using ShaderEffectId = uint32_t;
using ShaderBindingId = uint32_t;

struct ShaderEffectDesc {

    std::map<std::u8string, ShaderPackageData::ShaderInfo const*> shader_infos;
    std::unordered_map<ShaderBindingId, ShaderBindingDesc> shader_bindings;

    ShaderEffectDesc& set_shader_code(std::u8string const& name, ShaderPackageData::ShaderInfo const& shader_info, ShaderFlags const flags) {

        shader_infos[name] = &shader_info;
        return *this;
    }

    ShaderEffectDesc& set_binding(ShaderBindingId const id, ShaderBindingDesc const& shader_binding_desc) {

        shader_bindings[id] = shader_binding_desc;
        return *this;
    }
};

class ShaderEffect {
public:

    ShaderEffect() = default;

    std::span<Handle<backend::Shader> const> shaders() const;

    ShaderBindingDesc const& bindings(ShaderBindingId const id) const;

private:

    friend class ShaderEffectBinder;
    friend class ShaderCache;

    std::unordered_map<ShaderBindingId, ShaderBindingDesc> _bindings;
    std::vector<Handle<backend::Shader>> _shaders;
};

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

class ShaderCache {
public:

    ShaderCache() = default;

    void create_shader_effect(backend::Backend& backend, ShaderEffectId const id, ShaderEffectDesc const& shader_effect_desc);

    ShaderEffect const& shader_effect(ShaderEffectId const id) const;

    ShaderEffect& shader_effect(ShaderEffectId const id);

private:

    std::unordered_map<ShaderEffectId, ShaderEffect> _shader_effects;
    std::map<std::u8string, Handle<backend::Shader>> _shader_cache;
};
    
}
