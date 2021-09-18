// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/math/color.h"

namespace ionengine::renderer::fg {

class Resource {
friend class ResourceManager;
friend class FrameGraph;
public:

    Resource(const uint64 id, const ResourceType type, const std::string& name, api::View& view, const ResourceFlags flags) :
        m_id(id), m_type(type), m_name(name), m_view(view), m_flags(flags), m_ref_count(0) {

    }
    
    uint64 get_id() const { return m_id; }
    const std::string& get_name() const { return m_name; }
    ResourceFlags get_flags() const { return m_flags; }
    bool culling() const { return m_flags & ResourceFlags::None; }
    ResourceType get_type() const { return m_type; }

    api::View& get_view() { return m_view; }

    void write(Task& task) {
        m_writers.emplace_back(task);
    }

    void read(Task& task) {
        m_readers.emplace_back(task);
    }

    void debug_print() const {
        std::cout << format<char>("fg::Resource ID: {}, Name: {}, References: {}", m_id, m_name, m_ref_count) << std::endl;
    }

private:

    uint64 m_id;
    std::string m_name;
    ResourceType m_type;
    ResourceFlags m_flags;

    std::reference_wrapper<api::View> m_view;

    std::vector<std::reference_wrapper<Task>> m_writers;
    std::vector<std::reference_wrapper<Task>> m_readers;

    uint32 m_ref_count;
};

class ResourceManager {
friend class FrameGraph;
public:

    ResourceManager() : m_offset(0) {

    }

    ResourceHandle create(const std::string& name, const ResourceType type, api::View& view, const ResourceFlags flags) {

        m_resources.emplace_back(m_offset, ResourceType::Attachment, name, view, flags);
        m_resource_offsets[m_offset] = std::prev(m_resources.end());

        ResourceHandle handle(m_offset);
        m_offset++;
        return handle;
    }

    std::pair<ResourceHandle, std::optional<std::reference_wrapper<Resource>>> find_by_name(const std::string& name) {
        auto it = std::find_if(
            m_resources.begin(), 
            m_resources.end(),
            [&name](const Resource& element) {
                return name == element.get_name();
            }
        );
        return it != m_resources.end() ? std::make_pair<ResourceHandle, std::optional<std::reference_wrapper<Resource>>>(ResourceHandle { it->m_id }, *it) : std::make_pair<ResourceHandle, std::optional<std::reference_wrapper<Resource>>>(ResourceHandle::null(), std::nullopt);
    }

    std::optional<std::reference_wrapper<Resource>> get_by_handle(const ResourceHandle& handle) {
        auto it = m_resource_offsets.find(handle.m_id);
        return it != m_resource_offsets.end() ? std::optional<std::reference_wrapper<Resource>>{ *m_resource_offsets[handle.m_id] } : std::nullopt;
    }

private:

    uint64 m_offset;

    std::list<Resource> m_resources;
    std::map<uint64, std::list<Resource>::iterator> m_resource_offsets;
};

}