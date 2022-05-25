
#pragma once

#include <renderer/backend/backend.h>

using namespace ionengine::renderer {

class ShaderUniformBinder {
public:

    ShaderUniformBinder(backend::Device& device, ShaderProgram& shader_program) :
        _device(&device), _shader_program(&shader_program) {
    }

    void bind_buffer(uint32_t const index, GPUBuffer& buffer) {
        if(_updates.find(index) == _updates.end()) {
            _updates.insert({ index, static_cast<uint32_t>(_descriptor_writes.size()) });
            _descriptor_writes.emplace_back(index, buffer.as_buffer());
        } else {
            _descriptor_writes.at(_updates.at(index)) = backend::DescriptorWriteDesc { .index = index, .data = buffer.as_buffer() };
        }
    }

    void bind_texture(uint32_t const index, GPUTexture& texture) {
        if(_updates.find(index) == _updates.end()) {
            _updates.insert({ index, static_cast<uint32_t>(_descriptor_writes.size()) });
            _descriptor_writes.emplace_back(index, texture.as_texture());
            if(!texture.is_unordered_access()) {
                _descriptor_writes.emplace_back(index + 1, texture.as_sampler());
            }
        } else {
            _descriptor_writes.at(_updates.at(index)) = backend::DescriptorWriteDesc { .index = index, .data = texture.as_texture() };
            if(!texture.is_unordered_access()) {
                _descriptor_writes.at(_updates.at(index) + 1) = backend::DescriptorWriteDesc { .index = index + 1, .data = texture.as_sampler() };
            }
        }
    }

    void update(backend::Handle<backend::CommandList> const& command_list) {
        _device->bind_resources(command_list, _shader_program->descriptor_layout(), _descriptor_writes);
    }

    ShaderProgram& shader_program() { return *_shader_program; }

private:

    backend::Device* _device;
    ShaderProgram* _shader_program;

    std::unordered_map<uint32_t, uint32_t> _updates;
    std::vector<backend::DescriptorWriteDesc> _descriptor_writes;
};

}