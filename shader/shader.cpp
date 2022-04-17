// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <shader/shader.h>
#include <lib/exception.h>

using namespace ionengine::shader;

Shader::Shader(std::filesystem::path const& file_path) {

    std::string from_path_string = file_path.string();

    json5::error result = json5::from_file(from_path_string, _data);

    if(result != json5::error::none) {
        throw lib::Exception(std::format("File load {}!", from_path_string));
    }
}

ShaderData const& Shader::data() const {

    return _data;
}

