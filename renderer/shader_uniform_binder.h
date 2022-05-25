// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/shader_program.h>

namespace ionengine::renderer {

class ShaderUniformBinder {
public:

    ShaderUniformBinder(backend::Device& device, ShaderProgram& shader_program);

    void bind_resource(uint32_t const location, backend::ResourceHandle resource);

    void update(backend::Handle<backend::CommandList> const& command_list);

private:

    backend::Device* _device;
    ShaderProgram* _shader_program;

    std::unordered_map<uint32_t, uint32_t> _updates;
    std::vector<backend::DescriptorWriteDesc> _descriptor_writes;
};

}