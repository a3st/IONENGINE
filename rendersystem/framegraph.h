// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

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

    FrameGraphResource(const uint64 id) : m_id(id) {

    }

    FrameGraphResource() : m_id(std::numeric_limits<uint64>::max()) {

    }

    bool operator==(const FrameGraphResource& rhs) const { return m_id == rhs.m_id; }
    
    bool operator<(const FrameGraphResource& rhs) const { return m_id < rhs.m_id; }

    inline static FrameGraphResource null() { return FrameGraphResource(); }

private:

    uint64 m_id;
};



class FrameGraphResourceManager {
public:

    struct ResourceData {
        
    };

    FrameGraphResourceManager() {

    }

    FrameGraphResource create_resource(
        const FrameGraphResourceType resource_type,
        const gfx::Format format,
        const uint32 width,
        const uint32 height,
        const FrameGraphResourceFlags resource_flags
    ) {

        
        
        return {};
    }

    void delete_resource(const FrameGraphResource& resource) {

    }

private:

    std::map<uint64, FrameGraphResourceManager::ResourceData> m_resources;
};

class RenderPassBuilder {
public:

    RenderPassBuilder(FrameGraphResourceManager* resource_manager) : m_resource_manager(resource_manager) {

        assert(device && "pointer to resource_manager is null");
    }

    [[nodiscard]] FrameGraphResource create(
        const FrameGraphResourceType resource_type,
        const gfx::Format format,
        const uint32 width,
        const uint32 height,
        const FrameGraphResourceFlags resource_flags
    ) {        
        return m_resource_manager->create_resource(resource_type, format, width, height, resource_flags);
    }

    [[nodiscard]] FrameGraphResource write() {

        return {};
    }

    [[nodiscard]] FrameGraphResource read() {

        return {};
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

class FrameGraphPass {
public:

    FrameGraphPass() {

    }

private:

};

class FrameGraph {
public:

    FrameGraph(gfx::Device* device) : m_device(device) {

        assert(device && "pointer to device is null");


    }

    template<typename T>
    [[nodiscard]] FrameGraphPass add_pass(
        const std::string& pass_name, 
        const std::function<void(RenderPassBuilder*, T&)>& builder_func, 
        const std::function<void(RenderPassContext*, const T&)>& exec_func
    ) {
        T data{};

        return {};
    }

    template<typename T>
    [[nodiscard]] FrameGraphPass add_pass(
        const std::string& pass_name,
        const std::function<void(AsyncPassBuilder*, T&)>& builder_func, 
        const std::function<void(AsyncPassContext*, const T&)>& exec_func
    ) {
        T data{};

        return {};
    }

    void wait_pass(const FrameGraphPass& pass) {

    }

    void compile() {

    }

    void execute() {

    }

private:

    gfx::Device* m_device;

};

}