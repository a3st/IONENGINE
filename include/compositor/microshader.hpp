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
    std::string variable;
    ShaderParameterType variable_type;
};

class MicroShader {
public:

    MicroShader(std::filesystem::path const& file_path);

    MicroShader(MicroShader const& other);

    MicroShader(MicroShader&& other);

    auto operator=(MicroShader const& other) -> MicroShader&;

    auto operator=(MicroShader&& other) -> MicroShader&;

    auto get_shader_name() const -> std::string_view;

    auto get_entry_func() const -> std::string_view;

    auto get_in_params() const -> std::span<ShaderParameter const>;

    auto get_out_params() const -> std::span<ShaderParameter const>;

private:

    std::string shader_name;
    std::vector<ShaderParameter> in_params;
    std::vector<ShaderParameter> out_params;
    std::string entry_func;

    auto parse_shader(std::string_view const buffer) -> void;
};

}

}