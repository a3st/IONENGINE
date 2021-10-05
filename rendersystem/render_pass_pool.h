// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::rendersystem {

class RenderPassPool {
public:

    struct Key {
        std::vector<gfx::RenderPassColorDesc> colors;
        gfx::RenderPassDepthStencilDesc depth_stencil;
        uint32 sample_count;

        bool operator<(const Key& rhs) const {
            return std::tie(colors, depth_stencil, sample_count) < std::tie(rhs.colors, rhs.depth_stencil, rhs.sample_count);
        }
    };

    RenderPassPool(gfx::Device* device) : m_device(device) {

        assert(device && "pointer to device is null");
    }

    gfx::RenderPass* get_render_pass(const std::vector<gfx::RenderPassColorDesc>& colors, const std::optional<gfx::RenderPassDepthStencilDesc>& depth_stencil, const uint32 sample_count) {

        Key key = {
            colors,
            depth_stencil.has_value() ? depth_stencil.value() : gfx::RenderPassDepthStencilDesc {},
            sample_count
        };

        auto it = m_render_passes.find(key);
        if(it != m_render_passes.end()) {
            return it->second.get();
        } else {
            gfx::RenderPassDesc render_pass_desc = {
                key.colors,
                key.depth_stencil,
                key.sample_count
            };
            
            auto result = m_render_passes.emplace(key, m_device->create_render_pass(render_pass_desc));
            return result.first->second.get();
        }
    }

    void clear() {
        m_render_passes.clear();
    }

private:

    gfx::Device* m_device;

    std::map<RenderPassPool::Key, std::unique_ptr<gfx::RenderPass>> m_render_passes;
};

}