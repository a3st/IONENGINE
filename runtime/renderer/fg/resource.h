// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::fg {

enum class ResourceType {
    Attachment,
    Buffer
};

enum class ResourceFlags : uint32 {
    None = 1 << 0,
    Present = 1 << 1,
    DepthStencil = 1 << 2
};

ENUM_CLASS_BIT_FLAG_DECLARE(ResourceFlags)

struct AttachmentDesc {
    api::Format format;
    api::RenderPassLoadOp load_op;
    api::ClearValueColor clear_color;

    bool operator<(const AttachmentDesc& rhs) const {
        return std::tie(format, load_op, clear_color) < std::tie(format, rhs.load_op, rhs.clear_color);
    }
};

class Resource {
friend class ResourceManager;
public:

    Resource(const uint64 id, const ResourceType type, const std::string& name, api::View& view, const ResourceFlags flags) :
        m_id(id), m_type(type), m_name(name), m_view(view), m_flags(flags), m_ref_count(0) {

    }

    void acquire() {

        state_acquire();
        m_ref_count++;
    }

    void release() {

        state_release();
        m_ref_count--;
    }

    void clear() {

        m_states.clear();
        state_release();
        m_ref_count = 0;
    }

    std::pair<api::ResourceState, api::ResourceState> get_state() const { 
        return { m_states[m_states.size() - 2], m_states[m_states.size() - 1] };
    }
    
    uint64 get_id() const { return m_id; }
    const std::string& get_name() const { return m_name; }

    api::View& get_view() { return m_view; }

    void debug_print() const {

        std::cout << format<char>("FGResource ID: {}, Name: {}, References: {}\nStates:", m_id, m_name, m_ref_count);
        for(auto& state : m_states) {
            if(state == api::ResourceState::PixelShaderResource) std ::cout << " pixel_shader_resource ";
            if(state == api::ResourceState::RenderTarget) std ::cout << " render_target ";
            if(state == api::ResourceState::Present) std::cout << " present ";
        }
        std::cout << std::endl;
    }

private:

    uint64 m_id;
    std::string m_name;
    ResourceType m_type;
    ResourceFlags m_flags;

    std::reference_wrapper<api::View> m_view;
    std::vector<api::ResourceState> m_states;

    uint32 m_ref_count;

    void state_acquire() {
        switch(m_type) {
            case ResourceType::Attachment: {
                m_states.emplace_back(api::ResourceState::RenderTarget);
                break;
            }
        }
    }

    void state_release() {

        if(m_flags & ResourceFlags::Present) {
            m_states.emplace_back(api::ResourceState::Present);
        } else {
            switch(m_type) {
                case ResourceType::Attachment: {
                    m_states.emplace_back(api::ResourceState::PixelShaderResource);
                    break;
                }
            }
        }
    }
};

class ResourceHandle {
friend class ResourceManager;
public:

    ResourceHandle() {

    }

    ResourceHandle(const uint64 id) : m_id(id) {

    }

    bool operator==(const ResourceHandle& rhs) const { return m_id == rhs.m_id; }
    
    static ResourceHandle null() {
        return { std::numeric_limits<uint64>::max() };
    }

protected:

    uint64 get_id() const { return m_id; }

private:

    uint64 m_id;
};

class ResourceManager {
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

    ResourceHandle find_handle_by_name(const std::string& name) {

        auto it = std::find_if(
            m_resources.begin(), 
            m_resources.end(),
            [&name](const Resource& element) {
                return name == element.get_name();
            }
        );

        return it != m_resources.end() ? ResourceHandle { it->get_id() } : ResourceHandle::null();
    }

    Resource& get_resource(const ResourceHandle& handle) {
        return *m_resource_offsets[handle.get_id()];
    }

    std::list<Resource>& get_resources() { return m_resources; }

private:

    uint64 m_offset;

    std::list<Resource> m_resources;
    std::map<uint64, std::list<Resource>::iterator> m_resource_offsets;
};

}