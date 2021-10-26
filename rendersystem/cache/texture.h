// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class TextureCache {

    using Key = lgfx::TextureDesc;

public:

    TextureCache(lgfx::Device* device);
    TextureCache(const TextureCache&) = delete;
    TextureCache(TextureCache&&) = delete;

    TextureCache& operator=(const TextureCache&) = delete;
    TextureCache& operator=(TextureCache&&) = delete;

    lgfx::Texture* GetTexture(const Key& key);
    void Clear();

    inline void Reset() {

        textures_.clear();
    }

private:

    lgfx::Device* device_;

    struct TextureEntry {
        uint32_t entry_index;
        std::vector<std::unique_ptr<lgfx::Texture>> textures;
    };

    std::map<Key, TextureEntry> textures_;

    lgfx::MemoryPool rtds_memory_pool_;
    lgfx::MemoryPool sr_memory_pool_;
};
    
}
