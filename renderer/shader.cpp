// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader.h>

using namespace ionengine::renderer;
using ionengine::Handle;

ShaderSet::ShaderSet(backend::Backend& backend, ShaderSetCreateInfo const& create_info) 
    : _shaders(create_info.shaders), _bindings(create_info.bindings) {
    
}

std::span<Handle<backend::Shader> const> ShaderSet::shaders() const {

    return _shaders;
}

ShaderBinding const& ShaderSet::bindings(uint32_t const id) const {
    
    return _bindings.at(id); 
}

/*ShaderEffectBinder::ShaderEffectBinder(ShaderEffect& shader_effect) : _shader_effect(&shader_effect) {

}

ShaderEffectBinder& ShaderEffectBinder::bind(ShaderBindingId const id, std::variant<Handle<backend::Texture>, Handle<backend::Buffer>, Handle<backend::Sampler>> const& target) {

    auto binding_visitor = make_visitor(
        [&](ShaderBinding<Sampler> const& binding) {
            _descriptor_updates[_update_count] = backend::DescriptorWriteDesc { .index = binding.index, .data = target };
        },
        [&](ShaderBinding<Buffer> const& binding) {
            _descriptor_updates[_update_count] = backend::DescriptorWriteDesc { .index = binding.index, .data = target };
        },
        [&](ShaderBinding<Texture> const& binding) {
            _descriptor_updates[_update_count] = backend::DescriptorWriteDesc { .index = binding.index, .data = target };
        }
    );

    std::visit(binding_visitor, _shader_effect->_bindings[id]);
    ++_update_count;
    return *this;
}

void ShaderEffectBinder::update(backend::Backend& backend, Handle<backend::DescriptorSet> const& descriptor_set) {

    backend.update_descriptor_set(descriptor_set, std::span<DescriptorWriteDesc const>(_descriptor_updates.data(), _update_count));
    _update_count = 0;
}*/
