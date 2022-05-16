// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <asset/texture.h>

using namespace ionengine;
using namespace ionengine::asset;

Texture::Texture() {

}

lib::Expected<Texture, lib::Result<TextureError>> Texture::load_from_file(std::filesystem::path const& file_path) {

    return lib::Expected<Texture, lib::Result<TextureError>>::ok(Texture());
}
