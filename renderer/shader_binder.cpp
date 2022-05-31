// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader_binder.h>
#include <renderer/shader_program.h>

using namespace ionengine;
using namespace ionengine::renderer;

ShaderBinder::ShaderBinder(ShaderProgram& shader_program) :
    _descriptor_layout(shader_program.descriptor_layout) {

}

void ShaderBinder::update_resource(uint32_t const location, backend::ResourceHandle resource) {
    if(_exist_updates.find(location) == _exist_updates.end()) {
        _exist_updates.insert({ location, static_cast<uint32_t>(_update_resources.size()) });
        _update_resources.emplace_back(location, resource);
    } else {
        _update_resources.at(_exist_updates.at(location)) = backend::DescriptorWriteDesc { .index = location, .data = resource };
    }
}

void ShaderBinder::bind(backend::Device& device, backend::Handle<backend::CommandList> const& command_list) {
    device.bind_resources(command_list, _descriptor_layout, _update_resources);
}
