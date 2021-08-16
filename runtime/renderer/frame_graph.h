// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class FrameGraphResource {
public:

    FrameGraphResource(View& view) : m_view(view) {

    }

    View& get_view() const { return m_view; }

private:

    std::reference_wrapper<View> m_view;
};


class RenderPassResources {
public:

    RenderPassResources(Device& device) : m_device(device) {
        
    
    }

private:

    std::reference_wrapper<Device> m_device;
};

class RenderPassBuilder {
public:

    RenderPassBuilder(RenderPassResources& resources) : m_pass_resources(resources) {
        

    }

    // add_output
    // add_input
    

private:

    std::reference_wrapper<RenderPassResources> m_pass_resources;

};

class RenderPassContext {
public:

    RenderPassContext(Device& device) {

    }

private:


};

struct FrameGraphDesc {
    std::vector<std::reference_wrapper<Resource>> frames;
};

class FrameGraph {
public:

    FrameGraph(Device& device) : m_device(device)  {
        
    }

    void bind_attachment(const std::string& name, View& view) {

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