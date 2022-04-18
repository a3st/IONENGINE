// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/frontend/context.h>
#include <asset/technique.h>

namespace ionengine::renderer::frontend {

class ShaderProgram {
public:

    ShaderProgram(Context& context, asset::Technique const& technique);

    ~ShaderProgram();

private:

    Context* _context;

    std::vector<backend::Handle<backend::Shader>> _shaders;
    backend::Handle<backend::DescriptorLayout> _layout;

    backend::ShaderFlags get_shader_flags(asset::ShaderFlags const shader_flags);
};

}