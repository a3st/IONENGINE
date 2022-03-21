// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/data.h>

namespace ionengine::renderer {

using TextureId = uint32_t;

class TextureManager {
public:

    TextureManager() = default;

    void load_texture_data(TextureId const id, TextureData const& texture_data);


private:

};
    
}