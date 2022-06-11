// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader_binder.h>
#include <renderer/shader.h>

using namespace ionengine;
using namespace ionengine::renderer;

ShaderBinder::ShaderBinder(Shader& shader, NullData& null) :
    _descriptor_layout(shader.descriptor_layout) {

    for(auto const [uniform_name, uniform_data] : shader.uniforms) {
        
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

void ShaderBinder::bind(backend::Device& device, backend::Handle<backend::CommandList> const& command_list) {
    device.bind_resources(command_list, _descriptor_layout, _update_resources);
}
