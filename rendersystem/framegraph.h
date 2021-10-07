// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/math.h"

#include "texture.h"

#include "frame_buffer_pool.h"
#include "render_pass_pool.h"

namespace ionengine::rendersystem {

enum class FrameGraphResourceType {
    Attachment,
    Buffer
};

enum class FrameGraphResourceFlags : uint32 {
    None = 1 << 0,
    Swapchain = 1 << 1
};

ENUM_CLASS_BIT_FLAG_DECLARE(FrameGraphResourceFlags)

class FrameGraphResource {
public:

    FrameGraphResource(gfx::Device* device) : m_device(device) {

        assert(device && "pointer to device is null");

        m_texture = std::make_unique<Texture>(m_device, "swapchain_texture");

        m_resource_state = gfx::ResourceState::Present;
    }

    void create_from_swapchain() {

        m_texture->create_from_swapchain(m_device->get_swapchain_buffer_index()).value();
    }

    void set_resource_state(const gfx::ResourceState state) { m_resource_state = state; }

    gfx::ResourceState get_resource_state() const { return m_resource_state; }

    Texture* get_texture() const { return m_texture.get(); }

private:

    gfx::Device* m_device;

    std::unique_ptr<Texture> m_texture;

    gfx::ResourceState m_resource_state;
};

class FrameGraphResourcePool {
public:

    struct Key {
        bool acquire;
        uint32 frame_index;
        std::unique_ptr<FrameGraphResource> resource;
        FrameGraphResourceFlags resource_flags;
    };

    FrameGraphResourcePool(gfx::Device* device) : m_device(device) {

        assert(device && "pointer to device is null");
    }

    FrameGraphResource* get_resource(
        const FrameGraphResourceType resource_type,
        const Texture::Format format,
        const uint32 width,
        const uint32 height,
        const FrameGraphResourceFlags resource_flags
    ) {

        FrameGraphResource* ptr = nullptr;

        for(uint32 i = 0; i < m_resources.size(); ++i) {

            if(!m_resources[i].acquire && 
                m_resources[i].resource_flags == resource_flags &&
                m_resources[i].frame_index == m_device->get_swapchain_buffer_index()) {

                ptr = m_resources[i].resource.get();
                break;
            }
        }

        if(!ptr) {

            FrameGraphResourcePool::Key key = {
                true,
                m_device->get_swapchain_buffer_index(),
                std::make_unique<FrameGraphResource>(m_device),
                resource_flags
            };

            if(resource_flags & FrameGraphResourceFlags::Swapchain) {
                key.resource->create_from_swapchain();
            }

            ptr = key.resource.get();

            m_resources.emplace_back(std::move(key));
        }
        
        return ptr;
    }

    void release(FrameGraphResource* resource_) {

        for(auto& resource : m_resources) {

            if(resource_ == resource.resource.get()) {

                resource.acquire = false;
                break;
            }
        }
    }

private:

    gfx::Device* m_device;

    std::vector<FrameGraphResourcePool::Key> m_resources;
};

enum class FrameGraphResourceOp {
    Load,
    Clear
};

class FrameGraphPassContext {
public:

    FrameGraphPassContext() {

    }

private:

};

struct WriteFrameGraphResource {

    FrameGraphResource* resource;
    FrameGraphResourceOp op;
    math::Fcolor clear_color;
};

struct ReadFrameGraphResource {

    FrameGraphResource* resource;
};

class FrameGraphPass {
public:

    FrameGraphPass(
        const std::string& name,
        const std::function<void(FrameGraphPassContext*)>& exec_func
    ) : 
        m_name(name),
        m_exec_func(exec_func) {

    }

    void execute(FrameGraphPassContext* context) const { m_exec_func(context); }

    void read_resource(const ReadFrameGraphResource& read_resource) { m_read_resources.emplace_back(read_resource); }
 
    void write_resource(const WriteFrameGraphResource& write_resource) { m_write_resources.emplace_back(write_resource); }

    void create_resource(FrameGraphResource* resource) { m_create_resources.emplace_back(resource); }

    const std::vector<WriteFrameGraphResource>& get_write_resources() const { return m_write_resources; }

    const std::vector<ReadFrameGraphResource>& get_read_resources() const { return m_read_resources; }
    
    const std::vector<FrameGraphResource*>& get_create_resources() const { return m_create_resources; }

private:

    const std::string m_name;

    std::function<void(FrameGraphPassContext*)> m_exec_func;

    std::vector<WriteFrameGraphResource> m_write_resources;

    std::vector<ReadFrameGraphResource> m_read_resources;

    std::vector<FrameGraphResource*> m_create_resources;
};

class FrameGraphPassBuilder {
public:

    FrameGraphPassBuilder(FrameGraphResourcePool* resource_pool, FrameGraphPass* frame_graph_pass) 
        : 
            m_resource_pool(resource_pool), m_frame_graph_pass(frame_graph_pass) {

        assert(resource_pool && "pointer to resource_pool is null");
    }

    [[nodiscard]] FrameGraphResource* create(
        const FrameGraphResourceType resource_type,
        const Texture::Format format,
        const uint32 width,
        const uint32 height,
        const FrameGraphResourceFlags resource_flags
    ) {

        FrameGraphResource* resource = m_resource_pool->get_resource(resource_type, format, width, height, resource_flags);
        m_frame_graph_pass->create_resource(resource);
        return resource;
    }

    [[nodiscard]] FrameGraphResource* write(
        FrameGraphResource* resource, 
        const FrameGraphResourceOp op, 
        const math::Fcolor& clear_color = { 0.0f, 0.0f, 0.0f, 0.0f }
    ) {
        
        m_frame_graph_pass->write_resource(WriteFrameGraphResource { resource, op, clear_color });
        return resource;
    }

    [[nodiscard]] FrameGraphResource* read(FrameGraphResource* resource) {
        
        m_frame_graph_pass->read_resource(ReadFrameGraphResource { resource });
        return resource;
    }

private:

    FrameGraphResourcePool* m_resource_pool;

    FrameGraphPass* m_frame_graph_pass;
};

class FrameGraph {
public:

    FrameGraph(gfx::Device* device) : m_device(device) {

        assert(device && "pointer to device is null");

        m_resource_pool = std::make_unique<FrameGraphResourcePool>(device);
        m_frame_buffer_pool = std::make_unique<FrameBufferPool>(device);
        m_render_pass_pool = std::make_unique<RenderPassPool>(device);
    }

    template<typename T>
    [[nodiscard]] T add_pass(
        const std::string& pass_name, 
        const std::function<void(FrameGraphPassBuilder*, T&)>& builder_func, 
        const std::function<void(FrameGraphPassContext*, const T&)>& exec_func
    ) {
        T data{};

        m_passes.emplace_back(pass_name, std::bind(exec_func, std::placeholders::_1, data));

        FrameGraphPassBuilder builder(m_resource_pool.get(), &m_passes.back());
        builder_func(&builder, data);

        return data;
    }

    void execute(gfx::CommandList* present_command_list) {

        present_command_list->reset();

        FrameGraphPassContext context;

        for(auto& pass : m_passes) {

            std::vector<gfx::RenderPassColorDesc> render_pass_colors;
            std::vector<gfx::View*> color_views;
            std::vector<gfx::ResourceBarrierDesc> write_barriers;
            gfx::ClearValueDesc clear_desc;
            for(auto write_resource : pass.get_write_resources()) {
                
                auto src = write_resource.resource->get_resource_state();
                auto dst = gfx::ResourceState::RenderTarget;
                auto resource = write_resource.resource->get_texture()->get_resource();
                auto view = write_resource.resource->get_texture()->get_view();

                write_barriers.emplace_back(gfx::ResourceBarrierDesc { resource, src, dst });

                write_resource.resource->set_resource_state(dst);

                render_pass_colors.emplace_back(
                    gfx::RenderPassColorDesc { 
                        std::get<gfx::ResourceDesc>(resource->get_desc()).format,
                        render_pass_load_op_to_gfx(write_resource.op),
                        gfx::RenderPassStoreOp::Store
                    }
                );

                color_views.emplace_back(view);

                clear_desc.colors.emplace_back(
                    gfx::ClearValueColor { 
                        write_resource.clear_color.r,
                        write_resource.clear_color.g,
                        write_resource.clear_color.b,
                        write_resource.clear_color.a
                    }
                );
            }

            present_command_list->resource_barriers(write_barriers);

            gfx::RenderPass* render_pass = m_render_pass_pool->get_render_pass(render_pass_colors, std::nullopt, 1);
            gfx::FrameBuffer* frame_buffer = m_frame_buffer_pool->get_frame_buffer(render_pass, 800, 600, color_views, nullptr);

            present_command_list->begin_render_pass(render_pass, frame_buffer, clear_desc);
            pass.execute(&context);
            present_command_list->end_render_pass();

            for(auto resource : pass.get_create_resources()) {
                m_resource_pool->release(resource);
            }
        }

        for(auto write_resource : m_passes.back().get_write_resources()) {

            auto src = write_resource.resource->get_resource_state();
            auto dst = gfx::ResourceState::Present;
            auto resource = write_resource.resource->get_texture()->get_resource();

            present_command_list->resource_barriers({ { resource, src, dst } });

            write_resource.resource->set_resource_state(dst);
        }

        present_command_list->close();

        m_device->execute_command_lists(gfx::CommandListType::Graphics, { present_command_list });

        m_passes.clear();
    }

private:

    gfx::Device* m_device;

    std::vector<FrameGraphPass> m_passes;

    std::unique_ptr<FrameGraphResourcePool> m_resource_pool;

    std::unique_ptr<FrameBufferPool> m_frame_buffer_pool;

    std::unique_ptr<RenderPassPool> m_render_pass_pool;

    gfx::RenderPassLoadOp render_pass_load_op_to_gfx(const FrameGraphResourceOp op) {
        
        switch(op) {
            case FrameGraphResourceOp::Clear: return gfx::RenderPassLoadOp::Clear;
            case FrameGraphResourceOp::Load: return gfx::RenderPassLoadOp::Load;
            default: assert(false && "passed invalid argument to render_pass_load_op_to_gfx"); return gfx::RenderPassLoadOp::DontCare;
        }
    }
};

}