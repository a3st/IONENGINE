// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::rendersystem {

class FrameBufferCache {
public:

    struct Key {
        gfx::RenderPass* render_pass;
        uint32 width;
        uint32 height;
        std::vector<gfx::View*> color_views;
        gfx::View* depth_stencil_view;
        
        bool operator<(const Key& rhs) const {
            return std::tie(render_pass, width, height, color_views, depth_stencil_view) < 
                std::tie(rhs.render_pass, rhs.width, rhs.height, rhs.color_views, rhs.depth_stencil_view);
        }
    };

    FrameBufferCache(gfx::Device* device) : m_device(device) {
        
    }

    gfx::FrameBuffer* get_frame_buffer(gfx::RenderPass* render_pass, const uint32 width, const uint32 height, std::vector<gfx::View*> color_views, gfx::View* depth_stencil_view) {

        Key key = {
            render_pass,
            width,
            height,
            color_views,
            depth_stencil_view
        };

        auto it = m_frame_buffers.find(key);
        if(it != m_frame_buffers.end()) {
            return it->second.get();
        } else {
            gfx::FrameBufferDesc frame_buffer_desc = {
                key.render_pass,
                key.width,
                key.height,
                key.color_views,
                key.depth_stencil_view
            };

            auto result = m_frame_buffers.emplace(key, m_device->create_frame_buffer(frame_buffer_desc));
            return result.first->second.get();
        }
    }

    void clear() {
        m_frame_buffers.clear();
    }

private:

    gfx::Device* m_device;

    std::map<FrameBufferCache::Key, std::unique_ptr<gfx::FrameBuffer>> m_frame_buffers;
};

}