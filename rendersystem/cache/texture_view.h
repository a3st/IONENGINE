// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class TextureViewCache {

public:

    struct Key {

        lgfx::Texture* texture;
        lgfx::Dimension dimension;
        uint32_t base_mip_level;
        uint32_t mip_level_count;
        uint32_t base_array_layer;
        uint32_t array_layer_count;

        inline bool operator<(const Key& rhs) const {

            return std::tie(texture, dimension, base_mip_level, mip_level_count, base_array_layer, array_layer_count) < 
                std::tie(rhs.texture, rhs.dimension, rhs.base_mip_level, rhs.mip_level_count, rhs.base_array_layer, rhs.array_layer_count);
        }
    };

    TextureViewCache(lgfx::Device* device);
    TextureViewCache(const TextureViewCache&) = delete;
    TextureViewCache(TextureViewCache&&) = delete;

    TextureViewCache& operator=(const TextureViewCache&) = delete;
    TextureViewCache& operator=(TextureViewCache&&) = delete;

    lgfx::TextureView* GetTextureView(const Key& key);

    inline void Reset() {

        texture_views_.clear();
    }

private:

    lgfx::Device* device_;

    std::map<Key, std::unique_ptr<lgfx::TextureView>> texture_views_;

    lgfx::DescriptorPool rt_descriptor_pool_;
    lgfx::DescriptorPool ds_descriptor_pool_;
    lgfx::DescriptorPool sr_descriptor_pool_;
};
    
}
