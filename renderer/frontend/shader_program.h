// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/frontend/context.h>
#include <asset/technique.h>

namespace ionengine::renderer::frontend {

class ShaderProgram {
public:

    ShaderProgram(Context& context, asset::Technique const& technique);

    ShaderProgram(ShaderProgram const&) = delete;

    ShaderProgram(ShaderProgram&& other) noexcept;

    ShaderProgram& operator=(ShaderProgram const&) = delete;

    ShaderProgram& operator=(ShaderProgram&& other) noexcept;

    ~ShaderProgram();

    backend::Handle<backend::DescriptorLayout> descriptor_layout() const;

    std::span<backend::Handle<backend::Shader> const> shaders() const;

private:

    Context* _context;

    std::vector<backend::Handle<backend::Shader>> _shaders;
    backend::Handle<backend::DescriptorLayout> _descriptor_layout;

    std::unordered_map<std::string, uint32_t> _uniforms;

    backend::ShaderFlags constexpr get_shader_flags(asset::ShaderFlags const shader_flags) const;
};

}
