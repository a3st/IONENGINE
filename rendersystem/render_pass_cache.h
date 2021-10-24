// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class RenderPassCache {

public:

    RenderPassCache();
    RenderPassCache(lgfx::Device* device);
    RenderPassCache(const RenderPassCache&) = delete;
    RenderPassCache(RenderPassCache&& rhs) noexcept;

    RenderPassCache& operator=(const RenderPassCache&) = delete;
    RenderPassCache& operator=(RenderPassCache&& rhs) noexcept;

    lgfx::RenderPass* GetRenderPass(const lgfx::RenderPassDesc& desc);
    void Clear();

private:

    lgfx::Device* device_;

    std::map<lgfx::RenderPassDesc, lgfx::RenderPass> render_passes_;
};

}