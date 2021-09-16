// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class RenderPassCache {
public:

    struct Key {
        std::vector<api::RenderPassColorDesc> colors;
        api::RenderPassDepthStencilDesc depth_stencil;
        uint32 sample_count;

        bool operator<(const Key& rhs) const {
            return std::tie(colors, depth_stencil, sample_count) < std::tie(rhs.colors, rhs.depth_stencil, rhs.sample_count);
        }
    };

    RenderPassCache(api::Device& device) : m_device(device) {

    }

    api::RenderPass& get_render_pass(const Key& key) {

        auto it = m_render_passes.find(key);
        if(it != m_render_passes.end()) {
            return *it->second;
        } else {
            api::RenderPassDesc render_pass_desc = {
                key.colors,
                key.depth_stencil,
                key.sample_count
            };
            auto result = m_render_passes.emplace(key, m_device.get().create_render_pass(render_pass_desc));
            return *result.first->second;
        }
    }

    void clear() {
        m_render_passes.clear();
    }

private:

    std::reference_wrapper<api::Device> m_device;

    std::map<RenderPassCache::Key, std::unique_ptr<api::RenderPass>> m_render_passes;
};

class FrameBufferCache {
public:

    struct Key {
        std::reference_wrapper<api::RenderPass> render_pass;
        uint32 width;
        uint32 height;
        std::vector<std::reference_wrapper<api::View>> colors;
        std::optional<std::reference_wrapper<api::View>> depth_stencil;

        bool operator<(const Key& rhs) const {
            return std::tie(width, height) < std::tie(width, height);
        }
    };

    FrameBufferCache(api::Device& device) : m_device(device) {
        
    }

    api::FrameBuffer& get_frame_buffer(const Key& key) {

        auto it = m_frame_buffers.find(key);
        if(it != m_frame_buffers.end()) {
            return *it->second;
        } else {

            /*api::FrameBufferDesc frame_buffer_desc = {
                key.render_pass,
                key.width,
                key.height,
                key.colors
            };

            if(key.depth_stencil.has_value()) {
                frame_buffer_desc.depth_stencil = key.depth_stencil;
            }

            auto result = m_frame_buffers.emplace(key, m_device.get().create_frame_buffer(frame_buffer_desc));
*/
            return *result.first->second;
        }
    }

    void clear() {
        m_frame_buffers.clear();
    }

private:

    std::reference_wrapper<api::Device> m_device;

    std::map<FrameBufferCache::Key, std::unique_ptr<api::FrameBuffer>> m_frame_buffers;
};

}