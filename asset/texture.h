// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/math/vector.h>
#include <lib/expected.h>

namespace ionengine::asset {

enum class TextureError {
    IO,
    ParseError
};

class Texture {
public:

    static lib::Expected<Texture, lib::Result<TextureError>> load_from_file(std::filesystem::path const& file_path);

private:

    Texture();

    

};

}