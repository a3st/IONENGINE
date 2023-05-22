// Copyright © 2020-2023 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ie {

namespace compositor {

enum class ShaderResourceType {
    Sampler2D,
    Constant
};

enum class ShaderParameterType {
    Float,
    Float2,
    Float3,
    Float4,
    Float2x2,
    Float3x3,
    Float4x4,
    Boolean
};

struct ShaderParameter {
    std::string name;
    std::string description;
    std::string variable;
    ShaderParameterType variable_type;
};

}

}