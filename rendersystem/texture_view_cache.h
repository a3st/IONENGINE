// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class TextureViewCache {

    using Key = std::pair<lgfx::Texture*, lgfx::TextureViewDesc>;

public:

    TextureViewCache(lgfx::Device* device);

    lgfx::TextureView* GetTextureView(const Key& key);
    void Clear();

private:

    lgfx::Device* device_;

    std::map<Key, std::unique_ptr<lgfx::TextureView>> texture_views_;

    std::unique_ptr<lgfx::DescriptorPool> rt_descriptor_pool_;
    std::unique_ptr<lgfx::DescriptorPool> ds_descriptor_pool_;
    std::unique_ptr<lgfx::DescriptorPool> sr_descriptor_pool_;
};
    
}
