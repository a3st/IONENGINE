#pragma once

namespace ie {

namespace compositor {

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
    std::string semantic
};

class MicroShader {
public:

    MicroShader(std::filesystem::path const& file_path);



private:

    auto parse_shader(std::string_view const buffer) -> void;

};

}

}