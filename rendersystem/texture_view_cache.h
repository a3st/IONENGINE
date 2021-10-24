// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class TextureViewCache {

public:

    struct Key {

    };

    TextureViewCache();
    TextureViewCache(lgfx::Device* device);
    TextureViewCache(const TextureViewCache&) = delete;
    TextureViewCache(TextureViewCache&& rhs) noexcept;

    TextureViewCache& operator=(const TextureViewCache&) = delete;
    TextureViewCache& operator=(TextureViewCache&& rhs) noexcept;

    lgfx::TextureView* GetTextureView(const Key& key);
    void Clear();

private:

    
};
    
}
