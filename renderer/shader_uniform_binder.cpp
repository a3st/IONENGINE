// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/shader_uniform_binder.h>

using namespace ionengine;
using namespace ionengine::renderer;

ShaderUniformBinder::ShaderUniformBinder(backend::Device& device, ShaderProgram& shader_program) :
    _device(&device), _shader_program(&shader_program) {

}

void ShaderUniformBinder::bind_resource(uint32_t const location, backend::ResourceHandle resource) {

    if(_updates.find(location) == _updates.end()) {
        _updates.insert({ location, static_cast<uint32_t>(_descriptor_writes.size()) });
        _descriptor_writes.emplace_back(location, resource);
    } else {
        _descriptor_writes.at(_updates.at(location)) = backend::DescriptorWriteDesc { .index = location, .data = resource };
    }
}

void ShaderUniformBinder::update(backend::Handle<backend::CommandList> const& command_list) {
    _device->bind_resources(command_list, _shader_program->descriptor_layout, _descriptor_writes);
}
