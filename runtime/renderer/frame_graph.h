// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class FrameGraphResource {
public:

    FrameGraphResource() {

    }

private:


};


class RenderPassResources {
public:

    RenderPassResources() {
        
    }

    FrameGraphResource create_render_target(Resource& resource, const ViewDesc& view_desc) {

    }

private:

    std::vector<std::unique_ptr<DescriptorPool>> m_descriptor_pools;
    std::vector<std::unique_ptr<View>> m_views;
};

class RenderPassBuilder {
public:

    RenderPassBuilder(RenderPassResources& resources) : m_pass_resources(resources) {
        

    }

    FrameGraphResource create_render_target(Resource& resource) {
    
    }

private:

    std::reference_wrapper<RenderPassResources> m_pass_resources;

};

class RenderPassContext {
public:

    RenderPassContext() {

    }

private:


};

struct FrameGraphDesc {
    std::vector<std::reference_wrapper<Resource>> frames;
};

class FrameGraph {
public:

    FrameGraph(Device& device, const FrameGraphDesc& desc) : m_device(device), m_frame_index(0)  {
        
        m_frame_count = static_cast<uint32>(desc.frames.size());

        for(uint32 i = 0; i < m_frame_count; ++i) {
            
        }
    }

    template<typename T>
    void add_pass(
        const std::string& name, 
        const std::function<void(RenderPassBuilder&, const T&)>& setup_pass_func, 
        const std::function<void(RenderPassResources&, const T&, RenderPassContext&)>& exec_pass_func
    ) {

    }

    void build() {

    }

    void execute() {
        m_frame_index = (m_frame_index + 1) % m_frame_count;

        // std::cout << "frame_index: " << m_frame_index << std::endl;
    }

private:

    std::reference_wrapper<Device> m_device;

    std::unique_ptr<RenderPassBuilder> m_builder;
    
    std::vector<std::unique_ptr<RenderPassResources>> m_resources;
    std::vector<std::unique_ptr<RenderPassContext>> m_contexts;

    uint32 m_frame_index;
    uint32 m_frame_count;

};

}