// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/frontend/context.h>
#include <shader/technique.h>

namespace ionengine::renderer::frontend {

class ShaderProgram {
public:

    ShaderProgram(Context& context, shader::Technique const& technique);

private:

    std::vector<backend::Handle<backend::Shader>> _shaders;
};

}