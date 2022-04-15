
#pragma once

#include <json5/json5.hpp>
#include <json5/json5_input.hpp>
#include <json5/json5_reflect.hpp>

enum class UniformType {
    cbuffer,
    sampler2D
};

JSON5_ENUM(UniformType, cbuffer, sampler2D)

struct Uniform {
    std::string name;
    UniformType uniform_type;
    int _register;
};

JSON5_CLASS(Uniform, name, uniform_type, _register)


namespace ionengine::shader {

class TechniqueParser {
public:

    TechniqueParser() = default;

    bool parse(std::span<uint8_t const> const data);

private:
};

}