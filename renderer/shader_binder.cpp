// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader_binder.h>
#include <renderer/shader.h>
#include <renderer/command_list.h>

using namespace ionengine;
using namespace ionengine::renderer;

ShaderBinder::ShaderBinder(ResourcePtr<Shader> shader, NullData& null) : _shader(shader) {

    for(auto const [uniform_name, uniform_data] : shader->get().uniforms) {
        
        auto uniform_visitor = make_visitor(
            [&](ShaderUniformData<ShaderUniformType::Sampler2D> const& data) {
                _exist_updates.insert({ data.index, _update_resources.size() });
                _update_resources.emplace_back(data.index, null.texture->get().texture);
                _exist_updates.insert({ data.index + 1, _update_resources.size() });
                _update_resources.emplace_back(data.index + 1, null.texture->get().sampler);
            },
            [&](ShaderUniformData<ShaderUniformType::CBuffer> const& data) {
                _exist_updates.insert({ data.index, _update_resources.size() });
                _update_resources.emplace_back(data.index, null.cbuffer->get().buffer);
            },
            [&](ShaderUniformData<ShaderUniformType::SBuffer> const& data) {
                _exist_updates.insert({ data.index, _update_resources.size() });
                _update_resources.emplace_back(data.index, null.sbuffer->get().buffer);
            },
            [&](ShaderUniformData<ShaderUniformType::RWBuffer> const& data) {
                _exist_updates.insert({ data.index, _update_resources.size() });
                _update_resources.emplace_back(data.index, null.sbuffer->get().buffer);
            },
            [&](ShaderUniformData<ShaderUniformType::RWTexture2D> const& data) {
                _exist_updates.insert({ data.index, _update_resources.size() });
                _update_resources.emplace_back(data.index, null.texture->get().texture);
            }
        );

        std::visit(uniform_visitor, uniform_data.data);
    }
}

void ShaderBinder::update_resource(uint32_t const location, backend::ResourceHandle resource) {
    _update_resources.at(_exist_updates.at(location)) = backend::DescriptorWriteDesc { .index = location, .data = resource };
}

void ShaderBinder::bind(backend::Device& device, CommandList& command_list) {
    device.bind_resources(command_list.command_list, _shader->get().descriptor_layout, _update_resources);
}
