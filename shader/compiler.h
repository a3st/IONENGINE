#pragma once

namespace ionengine::shader {

class ShaderCompiler {
public:

    ShaderCompiler();

    bool compile(std::filesystem::path const& shader_path);

private:

    struct Impl;
    struct impl_deleter { void operator()(Impl* ptr) const; };
    std::unique_ptr<Impl, impl_deleter> _impl;
};

}