// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/math/color.h"

namespace ionengine::renderer::fg {

class RenderPassBuilder {
public:

    RenderPassBuilder(ResourceManager& resource_manager, Task& task) 
        : m_resource_manager(resource_manager), m_task(task) {
        
    }

    ResourceHandle read(const std::string& name) {
        auto result = m_resource_manager.get().find_by_name(name);
        result.second.value().get().read(m_task);
        m_task.get().read(result.first);
        return result.first;
    }

    ResourceHandle write(const std::string& name, const ResourceOp op, const math::Fcolor& clear_color = { 0, 0, 0, 0 }) {
        auto result = m_resource_manager.get().find_by_name(name);
        result.second.value().get().write(m_task);
        m_task.get().write(result.first, AttachmentDesc { op, clear_color });
        return result.first;
    }

private:

    std::reference_wrapper<ResourceManager> m_resource_manager;
    std::reference_wrapper<Task> m_task;
};

class ComputePassBuilder {
public:

    ComputePassBuilder(ResourceManager& resource_manager, Task& task) 
        : m_resource_manager(resource_manager), m_task(task) {
        
    }

    ResourceHandle read(const std::string& name) {
        auto result = m_resource_manager.get().find_by_name(name);
        result.second.value().get().read(m_task);
        m_task.get().read(result.first);
        return result.first;
    }

    ResourceHandle write(const std::string& name) {
        auto result = m_resource_manager.get().find_by_name(name);
        result.second.value().get().write(m_task);
        m_task.get().write(result.first, BufferDesc {});
        return result.first;
    }

private:

    std::reference_wrapper<ResourceManager> m_resource_manager;
    std::reference_wrapper<Task> m_task;
};

}