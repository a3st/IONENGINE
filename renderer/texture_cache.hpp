// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "texture.hpp"

namespace ionengine {

namespace renderer {

class TextureCache {
public:

    TextureCache(Backend& backend);

    auto get(TextureData const& data) -> core::ref_ptr<Texture>;

    auto update(float const dt) -> void;

private:

    Backend* backend;
};

}

}