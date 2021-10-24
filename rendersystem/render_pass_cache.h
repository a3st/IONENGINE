// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class RenderPassCache {

public:

    RenderPassCache(lgfx::Device* device);

    lgfx::RenderPass* GetRenderPass(const lgfx::RenderPassDesc& desc);
    void Clear();

private:

    lgfx::Device* device_;

    std::map<lgfx::RenderPassDesc, std::unique_ptr<lgfx::RenderPass>> render_passes_;
};

}