// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::api {

/**
    @brief Graphics API Shader class

    Shader class for Graphics API. It's needed for defining shaders (gpu programs)
*/
class Shader {
public:

    virtual ~Shader() = default;

    /**
        @brief Get the type of shader
        @return ShaderType type of shader
    */
    virtual ShaderType get_type() const = 0;
};

}