// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class FrameGraphResource {
public:

    FrameGraphResource() {

    }

private:


};

class RenderPassBuilder {
public:

    RenderPassBuilder() {
        

    }

    //FrameGraphResource read_

private:

};

class RenderPassResources {
public:

    RenderPassResources() {
        
    }

private:


};

class RenderPassContext {
public:

    RenderPassContext() {

    }

private:


};

class FrameGraph {
public:

    FrameGraph(Device& device) : m_device(device)  {

    }

    void set_static(const uint32 slot, Resource& resource) {

    }

    template<typename T>
    void add_pass(
        const std::string& name, 
        const std::function<void(RenderPassBuilder&, const T&)>& setup_pass_func, 
        const std::function<void(RenderPassResources&, const T&, RenderPassContext&)>& exec_pass_func
    ) {

    }

    void execute() {

    }

private:

    std::reference_wrapper<Device> m_device;

    std::unique_ptr<RenderPassBuilder> m_pass_builder;
    std::unique_ptr<RenderPassResources> m_pass_resources;
    std::unique_ptr<RenderPassContext> m_pass_context;

};

}