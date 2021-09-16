// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class RenderPassCache {
public:

    struct Key {
        std::vector<AttachmentDesc> colors;
        AttachmentDesc depth_stencil;
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
            api::RenderPassDesc render_pass_desc{};

            for(auto& color : key.colors) {
                api::RenderPassColorDesc color_desc{};
                color_desc.format = color.format;
                color_desc.load_op = color.load_op;
                color_desc.store_op = api::RenderPassStoreOp::Store;

                render_pass_desc.colors.emplace_back(color_desc);
            }

            api::RenderPassDepthStencilDesc depth_stencil_desc{};
            depth_stencil_desc.format = key.depth_stencil.format;
            depth_stencil_desc.depth_load_op = key.depth_stencil.load_op;
            depth_stencil_desc.stencil_load_op = key.depth_stencil.load_op;
            if(key.depth_stencil.load_op == api::RenderPassLoadOp::DontCare) {
                depth_stencil_desc.depth_store_op = api::RenderPassStoreOp::DontCare;
                depth_stencil_desc.stencil_store_op = api::RenderPassStoreOp::DontCare;
            } else {
                depth_stencil_desc.depth_store_op = api::RenderPassStoreOp::Store;
                depth_stencil_desc.stencil_store_op = api::RenderPassStoreOp::Store;
            }

            render_pass_desc.depth_stencil = depth_stencil_desc;
            render_pass_desc.sample_count = key.sample_count;

            auto result = m_render_passes.emplace(key, m_device.get().create_render_pass(render_pass_desc));

            std::cout << "RenderPass (" << renderer::api::get_api_name() << ") created" << std::endl;
            return *result.first->second;
        }
    }

    void clear() {
        m_render_passes.clear();
    }

private:

    std::reference_wrapper<api::Device> m_device;

    std::map<FrameGraphRenderPassCache::Key, std::unique_ptr<api::RenderPass>> m_render_passes;
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

    FrameGraphFrameBufferCache(api::Device& device) : m_device(device) {
        
    }

    api::FrameBuffer& get_frame_buffer(const Key& key) {

        auto it = m_frame_buffers.find(key);
        if(it != m_frame_buffers.end()) {
            return *it->second;
        } else {

            api::FrameBufferDesc frame_buffer_desc = {
                key.render_pass,
                key.width,
                key.height,
                key.colors
            };

            if(key.depth_stencil.has_value()) {
                frame_buffer_desc.depth_stencil = key.depth_stencil;
            }

            auto result = m_frame_buffers.emplace(key, m_device.get().create_frame_buffer(frame_buffer_desc));

            std::cout << "FrameBuffer (" << renderer::api::get_api_name() << ") created" << std::endl;
            return *result.first->second;
        }
    }

    void clear() {
        m_frame_buffers.clear();
    }

private:

    std::reference_wrapper<api::Device> m_device;

    std::map<FrameGraphFrameBufferCache::Key, std::unique_ptr<api::FrameBuffer>> m_frame_buffers;
};

}