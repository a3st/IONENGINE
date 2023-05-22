#pragma once

#include "core.hpp"

namespace ie {

namespace compositor {

class ShaderFunction {
public:

    ShaderFunction(std::filesystem::path const& file_path);

    ShaderFunction(ShaderFunction const& other);

    ShaderFunction(ShaderFunction&& other);

    auto operator=(ShaderFunction const& other) -> ShaderFunction&;

    auto operator=(ShaderFunction&& other) -> ShaderFunction&;

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

    auto parse_parameters(std::string_view const buffer, std::vector<ShaderParameter>& params) -> void;
};

}

}