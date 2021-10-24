// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class TextureCache {

public:

    struct Key {

    };

    TextureCache();
    TextureCache(lgfx::Device* device);
    TextureCache(const TextureCache&) = delete;
    TextureCache(TextureCache&& rhs) noexcept;

    TextureCache& operator=(const TextureCache&) = delete;
    TextureCache& operator=(TextureCache&& rhs) noexcept;

    lgfx::Texture* GetTexture(const Key& key);
    void Clear();

private:

    
};
    
}
