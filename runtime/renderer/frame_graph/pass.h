// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

struct AttachmentDesc {
    Format format;
    RenderPassLoadOp load_op;
    RenderPassStoreOp store_op;
    ClearValueColor clear_color;

    bool operator<(const AttachmentDesc& rhs) const {
        return std::tie(format, load_op, store_op, clear_color) < std::tie(format, rhs.load_op, rhs.store_op, rhs.clear_color);
    }
};

class FrameGraphRenderPass {
public:

    FrameGraphRenderPass(const std::string& name, std::function<void(RenderPassContext&)> exec_func)
        : m_name(name), m_exec_func(exec_func) {

    }

    const std::string& get_name() const { return m_name; }
    void execute(RenderPassContext& context) { m_exec_func(context); }

    void add_write(FrameGraphResource& resource, const RenderPassLoadOp load_op, const RenderPassStoreOp store_op, const ClearValueColor& clear_color) { 
        m_writes.emplace_back(resource); 
        m_attachments.emplace_back(AttachmentDesc { resource.get_view().get_resource().get_format(), load_op, store_op, clear_color });
    }

    void add_read(FrameGraphResource& resource) { m_reads.emplace_back(resource); }

    const std::vector<std::reference_wrapper<FrameGraphResource>>& get_writes() const { return m_writes; }
    const std::vector<std::reference_wrapper<FrameGraphResource>>& get_reads() const { return m_reads; }
    const std::vector<AttachmentDesc>& get_attachments() const { return m_attachments; }

private:

    std::string m_name;

    std::function<void(RenderPassContext&)> m_exec_func;

    std::vector<std::reference_wrapper<FrameGraphResource>> m_writes;
    std::vector<std::reference_wrapper<FrameGraphResource>> m_reads;

    std::vector<AttachmentDesc> m_attachments;
};

class FrameGraphRenderPassCache {
public:

    struct Key {
        std::vector<AttachmentDesc> colors;
        AttachmentDesc depth_stencil;
        uint32 sample_count;

        bool operator<(const Key& rhs) const {
            return std::tie(colors, depth_stencil, sample_count) < std::tie(rhs.colors, rhs.depth_stencil, rhs.sample_count);
        }
    };

    FrameGraphRenderPassCache(Device& device) : m_device(device) {

    }

    RenderPass& get_render_pass(const Key& key) {

        auto it = m_render_passes.find(key);
        if(it != m_render_passes.end()) {
            return *it->second;
        } else {

            RenderPassDesc render_pass_desc{};

            for(auto& color : key.colors) {
                RenderPassColorDesc color_desc{};
                color_desc.format = color.format;
                color_desc.load_op = color.load_op;
                color_desc.store_op = color.store_op;

                render_pass_desc.colors.emplace_back(color_desc);
            }

            RenderPassDepthStencilDesc depth_stencil_desc{};
            depth_stencil_desc.format = key.depth_stencil.format;
            depth_stencil_desc.depth_load_op = key.depth_stencil.load_op;
            depth_stencil_desc.depth_store_op = key.depth_stencil.store_op;
            depth_stencil_desc.stencil_load_op = key.depth_stencil.load_op;
            depth_stencil_desc.stencil_store_op = key.depth_stencil.store_op;

            render_pass_desc.depth_stencil = depth_stencil_desc;
            render_pass_desc.sample_count = key.sample_count;

            auto result = m_render_passes.emplace(key, m_device.get().create_render_pass(render_pass_desc));

            std::cout << "RenderPass (" << renderer::get_api_name() << ") created" << std::endl;
            return *result.first->second;
        }
    }

    void clear() {
        m_render_passes.clear();
    }

private:

    std::reference_wrapper<Device> m_device;

    std::map<FrameGraphRenderPassCache::Key, std::unique_ptr<RenderPass>> m_render_passes;
};

class FrameGraphFrameBufferCache {
public:

    struct Key {
        std::reference_wrapper<RenderPass> render_pass;
        uint32 width;
        uint32 height;
        std::vector<std::reference_wrapper<View>> colors;
        std::optional<std::reference_wrapper<View>> depth_stencil;

        bool operator<(const Key& rhs) const {
            return std::tie(width, height) < std::tie(width, height);
        }
    };

    FrameGraphFrameBufferCache(Device& device) : m_device(device) {
        
    }

    FrameBuffer& get_frame_buffer(const Key& key) {

        auto it = m_frame_buffers.find(key);
        if(it != m_frame_buffers.end()) {
            return *it->second;
        } else {

            FrameBufferDesc frame_buffer_desc = {
                key.render_pass,
                key.width,
                key.height,
                key.colors,
                key.depth_stencil
            };

            auto result = m_frame_buffers.emplace(key, m_device.get().create_frame_buffer(frame_buffer_desc));

            std::cout << "FrameBuffer (" << renderer::get_api_name() << ") created" << std::endl;
            return *result.first->second;
        }
    }

    void clear() {
        m_frame_buffers.clear();
    }

private:

    std::reference_wrapper<Device> m_device;

    std::map<FrameGraphFrameBufferCache::Key, std::unique_ptr<FrameBuffer>> m_frame_buffers;
};

}