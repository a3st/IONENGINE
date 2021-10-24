// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class TextureCache {

public:

    TextureCache(lgfx::Device* device);

    lgfx::Texture* GetTexture(const lgfx::TextureDesc& desc);
    void Clear();

private:

    lgfx::Device* device_;

    struct TextureEntry {
        uint32_t entry_index;
        std::vector<std::unique_ptr<lgfx::Texture>> textures;
    };

    std::map<lgfx::TextureDesc, TextureEntry> textures_;

    std::unique_ptr<lgfx::MemoryPool> rt_memory_pool_;
    std::unique_ptr<lgfx::MemoryPool> sr_memory_pool_;
};
    
}
