// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "framegraph_resource_pool.h"
#include "framegraph_pass_pool.h"
#include "frame_buffer_pool.h"
#include "render_pass_pool.h"

namespace ionengine::rendersystem {

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

        return nullptr;
    }

    [[nodiscard]] FrameGraphResource* create(
        const FrameGraphResourceType resource_type,
        Texture* texture,
        const FrameGraphResourceFlags resource_flags
    ) {

        auto resource = m_resource_pool->allocate(resource_type, texture, resource_flags);
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

        FrameGraphPassContext context(present_command_list);

        for(auto& pass : m_passes) {

            std::vector<gfx::RenderPassColorDesc> render_pass_colors;
            std::vector<gfx::View*> color_views;
            std::vector<gfx::ResourceBarrierDesc> write_barriers;
            gfx::ClearValueDesc clear_desc;
            for(auto write_resource : pass.get_write_resources()) {
                
                auto src = write_resource.resource->get_state();
                auto dst = gfx::ResourceState::RenderTarget;
                auto resource = write_resource.resource->get_texture()->get_resource();
                auto view = write_resource.resource->get_texture()->get_view();

                write_barriers.emplace_back(gfx::ResourceBarrierDesc { resource, src, dst });

                write_resource.resource->set_state(dst);

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
                m_resource_pool->deallocate(resource);
            }
        }

        for(auto write_resource : m_passes.back().get_write_resources()) {

            auto src = write_resource.resource->get_state();
            auto dst = gfx::ResourceState::Present;
            auto resource = write_resource.resource->get_texture()->get_resource();

            present_command_list->resource_barriers({ { resource, src, dst } });

            write_resource.resource->set_state(dst);
        }

        present_command_list->close();

        m_device->execute_command_lists(gfx::CommandListType::Graphics, { present_command_list });

        m_passes.clear();
    }

    void reset() {

        m_resource_pool->clear();
        m_frame_buffer_pool->clear();
        m_render_pass_pool->clear();
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