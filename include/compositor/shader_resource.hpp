// Copyright © 2020-2023 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core.hpp"

namespace ie {

namespace compositor {

class ShaderResource {
public:

    ShaderResource(ShaderResourceType const resource_type, ShaderParameterType const param_type);

    auto get_in_param() const -> std::optional<ShaderParameter>;

    auto get_out_param() const -> ShaderParameter;

private:
    
    std::optional<ShaderParameter> in_param;
    ShaderParameter out_param;
};

}

}