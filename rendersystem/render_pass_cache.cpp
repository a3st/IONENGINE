
#include "../precompiled.h"
#include "render_pass_cache.h"

using namespace ionengine::rendersystem;

RenderPassCache::RenderPassCache() {

}

RenderPassCache::RenderPassCache(lgfx::Device* device) {

}

RenderPassCache::RenderPassCache(RenderPassCache&& rhs) noexcept {

}

RenderPassCache& RenderPassCache::operator=(RenderPassCache&& rhs) noexcept {


    return *this;
}

lgfx::RenderPass* RenderPassCache::GetRenderPass(const Key& key) {

    
    return nullptr;
}

void RenderPassCache::Clear() {

    render_passes_.clear();
}