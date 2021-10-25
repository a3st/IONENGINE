// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class RenderPassCache {

    using Key = lgfx::RenderPassDesc;

public:

    RenderPassCache(lgfx::Device* device);

    lgfx::RenderPass* GetRenderPass(const Key& key);
    void Clear();

private:

    lgfx::Device* device_;

    std::map<Key, std::unique_ptr<lgfx::RenderPass>> render_passes_;
};

}