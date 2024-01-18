// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "texture.hpp"

namespace ionengine {

namespace renderer {

class TextureCache {
public:

    TextureCache(Context& context);

    auto get(TextureData const& data) -> core::ref_ptr<Texture>;

    auto update(float const dt) -> void;

private:

    Context* context;
    std::unordered_map<uint64_t, std::tuple<core::ref_ptr<Texture>, uint32_t>> entries;
};

}

}