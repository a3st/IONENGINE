// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader.h>

using namespace ionengine::renderer;

ShaderEffectBinder::ShaderEffectBinder(ShaderEffect& shader_effect) : _shader_effect(&shader_effect) {

}

ShaderEffectBinder& ShaderEffectBinder::bind(ShaderBindingId const id, std::variant<Handle<Texture>, Handle<Buffer>, Handle<Sampler>> const& target) {

    auto binding_visitor = make_visitor(
        [&](ShaderBinding<Sampler> const& binding) {
            _descriptor_updates[_update_count] = DescriptorWriteDesc { .index = binding.index, .data = target };
        },
        [&](ShaderBinding<Buffer> const& binding) {
            _descriptor_updates[_update_count] = DescriptorWriteDesc { .index = binding.index, .data = target };
        },
        [&](ShaderBinding<Texture> const& binding) {
            _descriptor_updates[_update_count] = DescriptorWriteDesc { .index = binding.index, .data = target };
        }
    );

    std::visit(binding_visitor, _shader_effect->bindings[id]);
    ++_update_count;
    return *this;
}

void ShaderEffectBinder::update(Backend& backend, Handle<DescriptorSet> const& descriptor_set) {

    backend.update_descriptor_set(descriptor_set, std::span<DescriptorWriteDesc const>(_descriptor_updates.data(), _update_count));
    _update_count = 0;
}

void ShaderCache::create_shader_effect(Backend& backend, ShaderEffectId const id, ShaderEffectDesc const& shader_effect_desc) {

    auto shader_effect = ShaderEffect {};

    for(auto const& [name, shader_info] : shader_effect_desc.shader_infos) {

        auto it = _shader_cache.find(name);
        if(it != _shader_cache.end()) {
            shader_effect.shaders.emplace_back(it->second);
        } else {
            Handle<Shader> shader = backend.create_shader(shader_info->data, shader_info->flags);
            shader_effect.shaders.emplace_back(shader);
            _shader_cache[name] = shader;
        }
    }

    _shader_effects[id] = std::move(shader_effect);
}

ShaderEffect const& ShaderCache::shader_effect(ShaderEffectId const id) const {

    return _shader_effects.at(id);
}

ShaderEffect& ShaderCache::shader_effect(ShaderEffectId const id) {

    return _shader_effects[id];
}
