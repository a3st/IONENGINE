
#include "precompiled.h"
#include "compositor/shader_function.hpp"

using namespace ie::compositor;

ShaderFunction::ShaderFunction(std::filesystem::path const& file_path) {
    std::fstream shader_stream(file_path, std::ios::in);

    if(!shader_stream.is_open()) {
        throw std::runtime_error(std::format("Can't find a shader function file '{}'", file_path.generic_string()));
    }

    std::stringstream buffer_stream;
    buffer_stream << shader_stream.rdbuf();

    parse_shader(buffer_stream.view());
}

auto ShaderFunction::parse_shader(std::string_view const buffer) -> void {
    size_t offset = buffer.find("// shader-function: ");
    if(offset != 0) {
        throw std::runtime_error("Shader function parse error (Unknown format)");
    }

    offset = std::string_view("// shader-function: ").size();

    shader_name = std::string_view(buffer.begin() + offset, buffer.begin() + buffer.find_first_of('\n', offset));

    if(shader_name.empty()) {
        throw std::runtime_error("Shader function parse error (Unknown shader name)");
    }

    auto shader_name_lower = shader_name | 
        std::views::transform([](auto const& ch) { return std::tolower(ch); }) | std::ranges::to<std::string>();

    entry_func = shader_name_lower + "_main";

    offset = buffer.find("struct " + shader_name_lower + "_in_t ");
    if(offset == std::string_view::npos) {
        throw std::runtime_error("Shader function parse error (Unknown input data)");
    }

    offset = buffer.find_first_of('{', offset) + 1;
    auto struct_data = std::string_view(buffer.begin() + offset, buffer.begin() + buffer.find_first_of('}', offset));

    parse_parameters(struct_data, in_params);

    offset = buffer.find("struct " + shader_name_lower + "_out_t ");
    if(offset == std::string_view::npos) {
        throw std::runtime_error("Shader function parse error (Unknown output data)");
    }

    offset = buffer.find_first_of('{', offset) + 1;
    struct_data = std::string_view(buffer.begin() + offset, buffer.begin() + buffer.find_first_of('}', offset));

    parse_parameters(struct_data, out_params);
}

auto ShaderFunction::parse_parameters(std::string_view const buffer, std::vector<ShaderParameter>& params) -> void {
    size_t offset = 0;
    while((offset = buffer.find("// serialize-field", offset)) != std::string_view::npos) {
        auto param = ShaderParameter {};

        offset = buffer.find('\n', offset) + 1;
        offset = buffer.find("// name: ", offset);
        offset = buffer.find_first_of('"', offset) + 1;
        param.name = std::string(buffer.begin() + offset, buffer.begin() + buffer.find_first_of('"', offset));

        offset = buffer.find('\n', offset) + 1;
        offset = buffer.find("// description: ", offset);
        offset = buffer.find_first_of('"', offset) + 1;
        param.description = std::string(buffer.begin() + offset, buffer.begin() + buffer.find_first_of('"', offset));

        offset = buffer.find('\n', offset) + 1;
        auto parameter_data = std::string_view(buffer.begin() + offset, buffer.begin() + buffer.find('\n', offset));

        size_t parameter_offset = 0;
        if((parameter_offset = parameter_data.find("float")) != std::string_view::npos) {
            param.variable_type = ShaderParameterType::Float;
        } else if((parameter_offset = parameter_data.find("float2")) != std::string_view::npos) {
            param.variable_type = ShaderParameterType::Float2;
        } else if((parameter_offset = parameter_data.find("float3")) != std::string_view::npos) {
            param.variable_type = ShaderParameterType::Float3;
        } else if((parameter_offset = parameter_data.find("float4")) != std::string_view::npos) {
            param.variable_type = ShaderParameterType::Float4;
        } else if((parameter_offset = parameter_data.find("float2x2")) != std::string_view::npos) {
            param.variable_type = ShaderParameterType::Float2x2;
        } else if((parameter_offset = parameter_data.find("float3x3")) != std::string_view::npos) {
            param.variable_type = ShaderParameterType::Float3x3;
        } else if((parameter_offset = parameter_data.find("float4x4")) != std::string_view::npos) {
            param.variable_type = ShaderParameterType::Float4x4;
        } else if((parameter_offset = parameter_data.find("bool")) != std::string_view::npos) {
            param.variable_type = ShaderParameterType::Boolean;
        } else {
            throw std::runtime_error("MicroShader parse error (Unknown parameter type)");
        }

        parameter_offset = parameter_data.find_first_of(' ', parameter_offset) + 1;
        param.variable = std::string_view(parameter_data.begin() + parameter_offset, parameter_data.begin() + parameter_data.find_first_of(';', parameter_offset));

        params.push_back(std::move(param));
    }
}

auto ShaderFunction::get_shader_name() const -> std::string_view {
    return std::string_view(shader_name.data(), shader_name.size());
}

auto ShaderFunction::get_entry_func() const -> std::string_view {
    return std::string_view(entry_func.data(), entry_func.size());
}

auto ShaderFunction::get_in_params() const -> std::span<ShaderParameter const> {
    return std::span<ShaderParameter const>(in_params.data(), in_params.size());
}

auto ShaderFunction::get_out_params() const -> std::span<ShaderParameter const> {
    return std::span<ShaderParameter const>(out_params.data(), out_params.size());
}
