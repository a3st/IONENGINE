// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/descriptor_binder.h>
#include <renderer/gpu_program.h>
#include <renderer/gpu_buffer.h>
#include <renderer/gpu_texture.h>
#include <renderer/command_list.h>

using namespace ionengine;
using namespace ionengine::renderer;

DescriptorBinder::DescriptorBinder(ResourcePtr<GPUProgram> program, NullData& null_data) : _program(program) {

    for(auto const [uniform_name, uniform_data] : _program->get().descriptors) {
        
        auto uniform_visitor = make_visitor(
            [&](ProgramDescriptorData<ProgramDescriptorType::Sampler2D> const& data) {
                _exist_updates.insert({ data.index, _update_resources.size() });
                _update_resources.emplace_back(data.index, null_data.texture->get().texture);
                _exist_updates.insert({ data.index + 1, _update_resources.size() });
                _update_resources.emplace_back(data.index + 1, null_data.texture->get().sampler);
            },
            [&](ProgramDescriptorData<ProgramDescriptorType::CBuffer> const& data) {
                _exist_updates.insert({ data.index, _update_resources.size() });
                _update_resources.emplace_back(data.index, null_data.cbuffer->get().buffer);
            },
            [&](ProgramDescriptorData<ProgramDescriptorType::SBuffer> const& data) {
                _exist_updates.insert({ data.index, _update_resources.size() });
                _update_resources.emplace_back(data.index, null_data.sbuffer->get().buffer);
            },
            [&](ProgramDescriptorData<ProgramDescriptorType::RWBuffer> const& data) {
                _exist_updates.insert({ data.index, _update_resources.size() });
                _update_resources.emplace_back(data.index, null_data.sbuffer->get().buffer);
            },
            [&](ProgramDescriptorData<ProgramDescriptorType::RWTexture2D> const& data) {
                _exist_updates.insert({ data.index, _update_resources.size() });
                _update_resources.emplace_back(data.index, null_data.texture->get().texture);
            }
        );

        std::visit(uniform_visitor, uniform_data.data);
    }
}

void DescriptorBinder::update(uint32_t const index, GPUTexture const& texture) {
    _update_resources.at(_exist_updates.at(index)) = backend::DescriptorWriteDesc { .index = index, .data = texture.texture };
    _update_resources.at(_exist_updates.at(index + 1)) = backend::DescriptorWriteDesc { .index = index + 1, .data = texture.sampler };
}

void DescriptorBinder::update(uint32_t const index, GPUBuffer const& buffer) {
    _update_resources.at(_exist_updates.at(index)) = backend::DescriptorWriteDesc { .index = index, .data = buffer.buffer };
}

void DescriptorBinder::bind(backend::Device& device, CommandList& command_list) {
    device.bind_resources(command_list.command_list, _program->get().descriptor_layout, _update_resources);
}
