#pragma once

namespace ie {

namespace compositor {

class MicroShader {
public:

    MicroShader(std::filesystem::path const& file_path);

private:

    auto parse_shader(std::string_view const buffer) -> void;

};

}

}