// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/data.h>
#include <lib/math/vector.h>

namespace ionengine::renderer {

template<class Type>
struct ShaderBinding {
    std::u8string name;
};

template<>
struct ShaderBinding<Sampler> {
    std::u8string name;
};

template<>
struct ShaderBinding<Buffer> {
    std::u8string name;
};

template<>
struct ShaderBinding<Texture> {
    std::u8string name;
};

using ShaderBindingDesc = std::variant<ShaderBinding<Sampler>, ShaderBinding<Buffer>,ShaderBinding<Texture>>;

using ShaderEffectId = uint32_t;
using ShaderBindingId = uint32_t;

struct ShaderEffectDesc {
    using ShaderBindingInfo = std::pair<uint32_t, ShaderBindingDesc>;

    std::map<std::u8string, ShaderPackageData::ShaderInfo const*> shader_infos;
    std::unordered_map<ShaderBindingId, ShaderBindingInfo> shader_bindings;

    ShaderEffectDesc& set_shader_code(std::u8string const& name, ShaderPackageData::ShaderInfo const& shader_info, ShaderFlags const flags) {

        shader_infos[name] = &shader_info;
        return *this;
    }

    ShaderEffectDesc& set_binding(ShaderBindingId const id, uint32_t const index, ShaderBindingDesc const& shader_binding_desc) {

        shader_bindings[id] = { index, shader_binding_desc };
        return *this;
    }
};

struct ShaderEffect {

    using ShaderBindingInfo = std::pair<uint32_t, ShaderBindingDesc>;

    std::unordered_map<ShaderBindingId, ShaderBindingInfo> bindings;

    std::unordered_map<ShaderFlags, Handle<Shader>> shaders;
};

class ShaderEffectBinder {
public:

    ShaderEffectBinder(ShaderEffect& shader_effect);

    ShaderEffectBinder& bind(ShaderBindingId const id, std::variant<Handle<Texture>, Handle<Buffer>, Handle<Sampler>> const& target);

    void update(Backend& backend, Handle<DescriptorSet> const& descriptor_set);

private:

    ShaderEffect* _shader_effect;

    std::array<DescriptorWriteDesc, 64> _descriptor_updates;
    uint32_t _update_count{0};
};

class ShaderCache {
public:

    ShaderCache() = default;

    void create_shader_effect(Backend& backend, ShaderEffectId const id, ShaderEffectDesc const& shader_effect_desc);

    ShaderEffect const& get_shader_effect(ShaderEffectId const id) const;

    ShaderEffect& get_shader_effect(ShaderEffectId const id);

private:

    std::unordered_map<ShaderEffectId, ShaderEffect> _shader_effects;
    std::map<std::u8string, Handle<Shader>> _shader_cache;
};
    
}
