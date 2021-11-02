// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../../precompiled.h"
#include "render_pass.h"

using namespace ionengine::rendersystem;

RenderPassCache::RenderPassCache(lgfx::Device* device) : device_(device) {

}

lgfx::RenderPass* RenderPassCache::GetRenderPass(const Key& key) {

    auto it = render_passes_.find(key);
    if(it != render_passes_.end()) {
        return it->second.get();
    } else {
        auto ret = render_passes_.emplace(key, std::make_unique<lgfx::RenderPass>(device_, key));
        return ret.first->second.get();
    }
}