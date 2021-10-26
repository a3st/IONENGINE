// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class RenderPassCache {

    using Key = lgfx::RenderPassDesc;

public:

    RenderPassCache(lgfx::Device* device);
    RenderPassCache(const RenderPassCache&) = delete;
    RenderPassCache(RenderPassCache&&) = delete;

    RenderPassCache& operator=(const RenderPassCache&) = delete;
    RenderPassCache& operator=(RenderPassCache&&) = delete;

    lgfx::RenderPass* GetRenderPass(const Key& key);

    inline void Reset() {

        render_passes_.clear();
    }

private:

    lgfx::Device* device_;

    std::map<Key, std::unique_ptr<lgfx::RenderPass>> render_passes_;
};

}