// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class TextureCache {

public:

    TextureCache();
    TextureCache(lgfx::Device* device);
    TextureCache(const TextureCache&) = delete;
    TextureCache(TextureCache&& rhs) noexcept;

    TextureCache& operator=(const TextureCache&) = delete;
    TextureCache& operator=(TextureCache&& rhs) noexcept;

    lgfx::Texture* GetTexture(const lgfx::TextureDesc& desc);
    void Clear();

private:

    lgfx::Device* device_;

    struct TextureEntry {
        uint32_t entry_index;
        std::vector<lgfx::Texture> textures;
    };

    std::map<lgfx::TextureDesc, TextureEntry> textures_;

    lgfx::MemoryPool rt_memory_pool_;
    lgfx::MemoryPool sr_memory_pool_;
};
    
}
