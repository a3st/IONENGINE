// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <asset/technique.h>

namespace ionengine::renderer {

class ShaderProgram {
public:

    ~ShaderProgram();

    static std::shared_ptr<ShaderProgram> from_technique(backend::Device& device, asset::Technique const& technique);

    backend::Handle<backend::DescriptorLayout> descriptor_layout() const;

    std::span<backend::Handle<backend::Shader> const> shaders() const;

private:

    ShaderProgram(backend::Device& device, asset::Technique const& technique);

    backend::Device* _device;

    std::vector<backend::Handle<backend::Shader>> _shaders;
    backend::Handle<backend::DescriptorLayout> _descriptor_layout;

    std::unordered_map<std::string, uint32_t> _uniforms;

    backend::ShaderFlags constexpr get_shader_flags(asset::ShaderFlags const shader_flags) const;
};

}
