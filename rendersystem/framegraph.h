// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/math.h"

namespace ionengine::rendersystem {

enum class FrameGraphResourceType {
    Attachment,
    Buffer
};

enum class FrameGraphResourceFlags {
    None,
    Swapchain
};

class FrameGraphResource {
public:

    FrameGraphResource() {

    }

private:

    std::unique_ptr<Texture> texture;
};

class FrameGraphResourceManager {
public:

    struct Key {
        
    };

    FrameGraphResourceManager() {

    }

    FrameGraphResource* get_resource(
        const FrameGraphResourceType resource_type,
        const Texture::Format format,
        const uint32 width,
        const uint32 height,
        const FrameGraphResourceFlags resource_flags
    ) {

        
        
        return nullptr;
    }

    void release_resource(const FrameGraphResource& resource) {

    }

private:

    std::map<Key, FrameGraphResource> m_resources;
};

enum class FrameGraphResourceOp {
    Load,
    Clear
};

class RenderPassBuilder {
public:

    RenderPassBuilder(FrameGraphResourceManager* resource_manager) : m_resource_manager(resource_manager) {

        assert(device && "pointer to resource_manager is null");
    }

    [[nodiscard]] FrameGraphResource* create(
        const FrameGraphResourceType resource_type,
        const Texture::Format format,
        const uint32 width,
        const uint32 height,
        const FrameGraphResourceFlags resource_flags
    ) {        
        return m_resource_manager->get_resource(resource_type, format, width, height, resource_flags);
    }

    [[nodiscard]] FrameGraphResource* write(FrameGraphResource* resource, const FrameGraphResourceOp op, const math::Fcolor& clear_color = { 0.0f, 0.0f, 0.0f, 0.0f }) {

        return nullptr;
    }

    [[nodiscard]] FrameGraphResource* read(FrameGraphResource* resource) {

        return nullptr;
    }

private:

    FrameGraphResourceManager* m_resource_manager;

};

class RenderPassContext {
public:

    RenderPassContext() {

    }

private:

};

class AsyncPassContext {
public:

    AsyncPassContext() {

    }

private:

};

class ComputePassBuilder {
public:

    ComputePassBuilder() {

    }

private:

};

class AsyncPassBuilder {
public:

    AsyncPassBuilder() {

    }

private:

};

template<typename T>
class FrameGraphPass {
public:

    FrameGraphPass() {

    }

    const T& get_data() { 
        return m_data;
    }

private:

    T m_data;
};

class FrameGraph {
public:

    FrameGraph(gfx::Device* device) : m_device(device) {

        assert(device && "pointer to device is null");


    }

    template<typename T>
    [[nodiscard]] FrameGraphPass<T> add_pass(
        const std::string& pass_name, 
        const std::function<void(RenderPassBuilder*, T&)>& builder_func, 
        const std::function<void(RenderPassContext*, const T&)>& exec_func
    ) {
        T data{};

        return {};
    }

    template<typename T>
    [[nodiscard]] FrameGraphPass<T> add_pass(
        const std::string& pass_name,
        const std::function<void(AsyncPassBuilder*, T&)>& builder_func, 
        const std::function<void(AsyncPassContext*, const T&)>& exec_func
    ) {
        T data{};

        return {};
    }

    //void wait_pass(const FrameGraphPass& pass) {

    //}

    void execute() {

        //std::cout << "execute framegraph" << std::endl;
    }

private:

    gfx::Device* m_device;

};

}