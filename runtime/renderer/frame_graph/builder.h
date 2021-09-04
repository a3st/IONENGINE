// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class RenderPassBuilder {
friend class FrameGraph;
public:

    RenderPassBuilder(FrameGraphResourceManager& resource_manager) : m_resource_manager(resource_manager) {
        
    }

    FrameGraphResourceHandle add_input(const std::string& name, const RenderPassLoadOp load_op, const RenderPassStoreOp store_op, const ClearValueColor& clear_value = { 0, 0, 0, 0 }) {
        return { };
    }

    FrameGraphResourceHandle add_output(const std::string& name, const RenderPassLoadOp load_op, const RenderPassStoreOp store_op, const ClearValueColor& clear_value = { 0, 0, 0, 0 }) {
        return { };
    }

private:

    std::reference_wrapper<FrameGraphResourceManager> m_resource_manager;
};

}