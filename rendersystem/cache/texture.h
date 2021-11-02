// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class TextureCache {

public:

    struct Key {

        lgfx::Dimension dimension;
        uint32_t width;
        uint32_t height;
        uint32_t mip_levels;
        uint32_t array_layers;
        lgfx::Format format;
        lgfx::TextureFlags flags;

        inline bool operator<(const Key& rhs) const {

            return std::tie(dimension, width, height, mip_levels, array_layers, format, flags) < 
                std::tie(rhs.dimension, rhs.width, rhs.height, rhs.mip_levels, rhs.array_layers, rhs.format, rhs.flags);
        }
    };

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
