// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <asset/technique.h>

namespace ionengine::renderer {

class ShaderUniformPool {
public:

    ShaderUniformPool() {
        
    }

private:

    std::vector<backend::Handle<backend::DescriptorSet>> _descriptor_sets;
    uint32_t _offset{0};

};

class ShaderProgram {
public:

    ShaderProgram(backend::Device& device, asset::Technique const& technique);

    ShaderProgram(ShaderProgram const&) = delete;

    ShaderProgram(ShaderProgram&& other) noexcept;

    ShaderProgram& operator=(ShaderProgram const&) = delete;

    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    ~ShaderProgram();

    backend::Handle<backend::DescriptorLayout> descriptor_layout() const;

    std::span<backend::Handle<backend::Shader> const> shaders() const;

private:

    backend::Device* _device;

    std::vector<backend::Handle<backend::Shader>> _shaders;
    backend::Handle<backend::DescriptorLayout> _descriptor_layout;

    std::unordered_map<std::string, uint32_t> _uniforms;

    backend::ShaderFlags constexpr get_shader_flags(asset::ShaderFlags const shader_flags) const;
};

}
