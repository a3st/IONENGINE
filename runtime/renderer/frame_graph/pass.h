// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

struct WriteAttachmentDesc {
    RenderPassLoadOp load_op;
    RenderPassStoreOp store_op;
    ClearValueColor clear_color;
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
        m_write_attachments.emplace_back(WriteAttachmentDesc { load_op, store_op, clear_color });
    }

    void add_read(FrameGraphResource& resource) { m_reads.emplace_back(resource); }

    const std::vector<std::reference_wrapper<FrameGraphResource>>& get_writes() const { return m_writes; }
    const std::vector<std::reference_wrapper<FrameGraphResource>>& get_reads() const { return m_reads; }

private:

    std::string m_name;

    std::function<void(RenderPassContext&)> m_exec_func;

    std::vector<std::reference_wrapper<FrameGraphResource>> m_writes;
    std::vector<std::reference_wrapper<FrameGraphResource>> m_reads;

    std::vector<WriteAttachmentDesc> m_write_attachments;
};

class FrameGraphRenderPassCache {
public:

    struct Key {
        
    };

    FrameGraphRenderPassCache(Device& device) : m_device(device) {

    }

private:

    std::reference_wrapper<Device> m_device;

    std::map<FrameGraphRenderPassCache::Key, std::unique_ptr<RenderPass>> m_render_passes;
};


class FrameGraphFrameBufferCache {
public:

    struct Key {
        
    };

    FrameGraphFrameBufferCache(Device& device) : m_device(device) {
        
    }

    FrameBuffer& get_frame_buffer(const FrameGraphFrameBufferCache::Key& key) {

        
        return *m_frame_buffers[key];
    }

private:

    std::reference_wrapper<Device> m_device;

    std::map<FrameGraphFrameBufferCache::Key, std::unique_ptr<FrameBuffer>> m_frame_buffers;

};

}