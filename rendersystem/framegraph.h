// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "framegraph_resource_pool.h"

#include "frame_buffer_cache.h"
#include "render_pass_cache.h"

namespace ionengine::rendersystem {

class FrameGraphPassContext {
public:

    FrameGraphPassContext(gfx::CommandList* command_list) : m_command_list(command_list) {


    }

    gfx::CommandList* get_command_list() { return m_command_list; }

private:

    gfx::CommandList* m_command_list;

};

struct FrameGraphPassDesc {
    std::string name;
    std::function<void(FrameGraphPassContext*)> exec_func;
    std::vector<FrameGraphResource*> creates;
    std::vector<WriteFrameGraphResource> writes;
    std::vector<ReadFrameGraphResource> reads;
};

class FrameGraphPassBuilder {
public:

    FrameGraphPassBuilder(FrameGraphResourcePool* resource_pool, FrameGraphPassDesc& pass_desc) 
        : m_resource_pool(resource_pool), m_pass_desc(pass_desc) {

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
        m_pass_desc.creates.emplace_back(resource);
        return resource;
    }

    [[nodiscard]] FrameGraphResource* write(
        FrameGraphResource* resource, 
        const FrameGraphResourceOp op, 
        const math::Fcolor& clear_color = { 0.0f, 0.0f, 0.0f, 0.0f }
    ) {
        
        m_pass_desc.writes.emplace_back(WriteFrameGraphResource { resource, op, clear_color });
        return resource;
    }

    [[nodiscard]] FrameGraphResource* read(FrameGraphResource* resource) {
        
        m_pass_desc.reads.emplace_back(ReadFrameGraphResource { resource });
        return resource;
    }

private:

    FrameGraphResourcePool* m_resource_pool;
    FrameGraphPassDesc& m_pass_desc;
};

class FrameGraph {
public:

    FrameGraph(gfx::Device* device) : m_device(device) {

        assert(device && "pointer to device is null");

        m_resource_pool = std::make_unique<FrameGraphResourcePool>(device);
        m_frame_buffer_cache = std::make_unique<FrameBufferCache>(device);
        m_render_pass_cache = std::make_unique<RenderPassCache>(device);
    }

    template<typename T>
    [[nodiscard]] T add_pass(
        const std::string& pass_name, 
        const std::function<void(FrameGraphPassBuilder*, T&)>& builder_func, 
        const std::function<void(FrameGraphPassContext*, const T&)>& exec_func
    ) {
        T data{};

        m_pass_descs.emplace_back(FrameGraphPassDesc { pass_name, std::bind(exec_func, std::placeholders::_1, data) });

        FrameGraphPassBuilder builder(m_resource_pool.get(), m_pass_descs.back());
        builder_func(&builder, data);

        return data;
    }

    void execute(gfx::CommandList* present_command_list) {

        present_command_list->reset();

        FrameGraphPassContext context(present_command_list);

        for(auto& pass : m_pass_descs) {

            std::vector<gfx::RenderPassColorDesc> render_pass_colors;
            std::vector<gfx::View*> color_views;
            std::vector<gfx::ResourceBarrierDesc> write_barriers;
            gfx::ClearValueDesc clear_desc;
            for(auto write_resource : pass.writes) {
                
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

            gfx::RenderPass* render_pass = m_render_pass_cache->get_render_pass(render_pass_colors, std::nullopt, 1);
            gfx::FrameBuffer* frame_buffer = m_frame_buffer_cache->get_frame_buffer(render_pass, 800, 600, color_views, nullptr);
    
            present_command_list->begin_render_pass(render_pass, frame_buffer, clear_desc);
            pass.exec_func(&context);
            present_command_list->end_render_pass();

            for(auto resource : pass.creates) {
                m_resource_pool->deallocate(resource);
            }
        }

        for(auto write_resource : m_pass_descs.back().writes) {

            auto src = write_resource.resource->get_state();
            auto dst = gfx::ResourceState::Present;
            auto resource = write_resource.resource->get_texture()->get_resource();

            present_command_list->resource_barriers({ { resource, src, dst } });

            write_resource.resource->set_state(dst);
        }

        present_command_list->close();

        m_device->execute_command_lists(gfx::CommandListType::Graphics, { present_command_list });

        m_pass_descs.clear();
    }

    void reset() {

        m_resource_pool->reset();
        m_frame_buffer_cache->clear();
        m_render_pass_cache->clear();
    }

private:

    gfx::Device* m_device;

    std::vector<FrameGraphPassDesc> m_pass_descs;

    std::unique_ptr<FrameGraphResourcePool> m_resource_pool;

    std::unique_ptr<FrameBufferCache> m_frame_buffer_cache;

    std::unique_ptr<RenderPassCache> m_render_pass_cache;

    gfx::RenderPassLoadOp render_pass_load_op_to_gfx(const FrameGraphResourceOp op) {
        
        switch(op) {
            case FrameGraphResourceOp::Clear: return gfx::RenderPassLoadOp::Clear;
            case FrameGraphResourceOp::Load: return gfx::RenderPassLoadOp::Load;
            default: assert(false && "passed invalid argument to render_pass_load_op_to_gfx"); return gfx::RenderPassLoadOp::DontCare;
        }
    }
};

}