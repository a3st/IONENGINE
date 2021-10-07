// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/math.h"
#include "lib/ref_ptr.h"

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
friend class FrameGraphResourcePool;
public:

    FrameGraphResource(gfx::Device* device) : m_device(device), m_ref_count(0) {

        assert(device && "pointer to device is null");

        m_texture = std::make_unique<Texture>(m_device, "swapchain_texture");
    }

    [[nodiscard]] lib::Expected<FrameGraphResource*, std::string> create_from_swapchain() {

        m_texture->create_from_swapchain(m_device->get_swapchain_buffer_index()).value();

        return lib::make_expected<FrameGraphResource*, std::string>(this);
    }

    Texture* get_texture() const { return m_texture.get(); }

private:

    gfx::Device* m_device;

    std::unique_ptr<Texture> m_texture;

    uint32 m_ref_count;
};

class FrameGraphResourcePool {
public:

    struct Key {
        bool acquire;
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
                m_resources[i].resource_flags == resource_flags) {

                ptr = m_resources[i].resource.get();
                break;
            }
        }

        if(!ptr) {

            FrameGraphResourcePool::Key key = {
                true,
                std::make_unique<FrameGraphResource>(m_device),
                resource_flags
            };

            if(resource_flags & FrameGraphResourceFlags::Swapchain) {
                key.resource->create_from_swapchain().value();
            }

            m_resources.emplace_back(std::move(key));
        }
        
        return ptr;
    }

    void release(FrameGraphResource* resource) {

        for(uint32 i = 0; i < m_resources.size(); ++i) {

            if(m_resources[i].resource.get() == resource) {

                m_resources[i].acquire = false;
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

    void read(const ReadFrameGraphResource& read_resource) {
        
        m_reads.emplace_back(read_resource);
    }

    void write(const WriteFrameGraphResource& write_resource) {

        m_writes.emplace_back(write_resource);
    }

    const std::vector<WriteFrameGraphResource>& get_writes() const { return m_writes; }

    const std::vector<ReadFrameGraphResource>& get_reads() const { return m_reads; }

private:

    const std::string m_name;

    std::function<void(FrameGraphPassContext*)> m_exec_func;

    std::vector<WriteFrameGraphResource> m_writes;
    std::vector<ReadFrameGraphResource> m_reads;
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
        
        return m_resource_pool->get_resource(resource_type, format, width, height, resource_flags);
    }

    [[nodiscard]] FrameGraphResource* write(
        FrameGraphResource* resource, 
        const FrameGraphResourceOp op, 
        const math::Fcolor& clear_color = { 0.0f, 0.0f, 0.0f, 0.0f }
    ) {
        
        m_frame_graph_pass->write(WriteFrameGraphResource { resource, op, clear_color });
        return resource;
    }

    [[nodiscard]] FrameGraphResource* read(FrameGraphResource* resource) {
        
        m_frame_graph_pass->read(ReadFrameGraphResource { resource });
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

    void execute(gfx::CommandList* command_list) {

        FrameGraphPassContext context;

        for(uint32 i = 0; i < m_passes.size(); ++i) {

            // std::cout << lib::format<char>("id {}, writes {}, reads {}", i, m_passes[i].get_writes().size(), m_passes[i].get_reads().size()) << std::endl;
            
            m_passes[i].execute(&context);
        }

        m_passes.clear();
    }

private:

    gfx::Device* m_device;

    std::unique_ptr<FrameGraphResourcePool> m_resource_pool;

    std::vector<FrameGraphPass> m_passes;

    std::unique_ptr<FrameBufferPool> m_frame_buffer_pool;

    std::unique_ptr<RenderPassPool> m_render_pass_pool;
};

}

/*namespace ionengine::lib {

template<>
class RefPtr<rendersystem::FrameGraphResource> {
public:

    RefPtr(rendersystem::FrameGraphResource* ptr) : m_ptr(ptr) {

    }

    ~RefPtr() { m_ptr->release(); }

    RefPtr(const RefPtr& rhs) : m_ptr(rhs.m_ptr) { m_ptr->acquire(); }
    
    RefPtr(RefPtr&& rhs) : m_ptr(rhs.m_ptr) { }

    RefPtr& operator=(const RefPtr& rhs) {

        m_ptr = rhs.m_ptr;
        m_ptr->acquire();
        return *this;
    }
    
    RefPtr& operator=(RefPtr&& rhs) {

        m_ptr = rhs.m_ptr;
        return *this;
    }

private:

    rendersystem::FrameGraphResource* m_ptr;
};*/