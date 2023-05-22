
#include "precompiled.h"
#include "compositor/shader_resource.hpp"

using namespace ie::compositor;

ShaderResource::ShaderResource(ShaderResourceType const resource_type, ShaderParameterType const param_type) {
    if(resource_type == ShaderResourceType::Sampler2D) {
        out_param = ShaderParameter {};
        out_param.name = "Color";
        out_param.description = "Texture color";
        out_param.variable = "sampler2D";
        switch(param_type) {
            case ShaderParameterType::Float3: {
                out_param.variable_type = ShaderParameterType::Float3;
            } break;

            case ShaderParameterType::Float4: {
                out_param.variable_type = ShaderParameterType::Float4;
            } break;

            default: {
                throw std::invalid_argument("Invalid shader parameter argument");
            } break;
        }
    } else if(resource_type == ShaderResourceType::Constant) {
        out_param = ShaderParameter {};
        out_param.name = "Value";
        out_param.description = "Value";
        out_param.variable = "constant";
        switch(param_type) {
            case ShaderParameterType::Float: {
                out_param.variable_type = ShaderParameterType::Float;
            } break;

            case ShaderParameterType::Float2: {
                out_param.variable_type = ShaderParameterType::Float2;
            } break;

            case ShaderParameterType::Float3: {
                out_param.variable_type = ShaderParameterType::Float3;
            } break;

            case ShaderParameterType::Float4: {
                out_param.variable_type = ShaderParameterType::Float4;
            } break;

            case ShaderParameterType::Boolean: {
                out_param.variable_type = ShaderParameterType::Boolean;
            } break;

            default: {
                throw std::invalid_argument("Invalid shader parameter argument");
            } break;
        }
    }
}

auto ShaderResource::get_in_param() const -> std::optional<ShaderParameter> {
    return in_param;
}

auto ShaderResource::get_out_param() const -> ShaderParameter {
    return out_param;
}