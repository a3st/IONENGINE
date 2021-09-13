// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class FrameGraphResourceType {
    Attachment,
    Buffer
};

enum class FrameGraphResourceFlags : uint32 {
    None = 1 << 0,
    Present = 1 << 1
};

FrameGraphResourceFlags operator|(const FrameGraphResourceFlags lhs, const FrameGraphResourceFlags rhs) {
	return static_cast<FrameGraphResourceFlags>(static_cast<uint32>(lhs) | static_cast<uint32>(rhs));
}

bool operator&(const FrameGraphResourceFlags lhs, const FrameGraphResourceFlags rhs) {
	return static_cast<uint32>(lhs) & static_cast<uint32>(rhs);
}

class FrameGraphResource {
friend class FrameGraphResourceManager;
public:

    FrameGraphResource(const uint64 id, const FrameGraphResourceType type, const std::string& name, View& view, const FrameGraphResourceFlags flags) :
        m_id(id), m_type(type), m_name(name), m_view(view), m_flags(flags), m_ref_count(0) {

    }

    void acquire() {

        switch(m_type) {
            case FrameGraphResourceType::Attachment: {
                m_states.emplace_back(ResourceState::RenderTarget);
                break;
            }
        }

        m_ref_count++;
    }

    void release() {

        if(m_flags & FrameGraphResourceFlags::Present) {
            m_states.emplace_back(ResourceState::Present);
        } else {
            switch(m_type) {
                case FrameGraphResourceType::Attachment: {
                    m_states.emplace_back(ResourceState::PixelShaderResource);
                    break;
                }
            }
        }

        m_ref_count--;
    }

    void clear() {

        m_states.clear();

        if(m_flags & FrameGraphResourceFlags::Present) {
            m_states.emplace_back(ResourceState::Present);
        } else {
            switch(m_type) {
                case FrameGraphResourceType::Attachment: {
                    m_states.emplace_back(ResourceState::PixelShaderResource);
                    break;
                }
            }
        }

        m_ref_count = 0;
    }

    ResourceState get_before_state() const {
        return m_states[m_states.size() - 2];
    }

    ResourceState get_after_state() const {
        return m_states.back();
    }

    void print_test() const {

        std::cout << "resource id: " << get_id() << " refs: " << m_ref_count << " states: " << std::endl;
        for(auto& state : m_states) {
            if(state == ResourceState::PixelShaderResource) std ::cout << " pixel_shader_resource ";
            if(state == ResourceState::RenderTarget) std ::cout << " render_target ";
            if(state == ResourceState::Present) std::cout << " present ";
        }
        std::cout << std::endl;
    }
    
    uint64 get_id() const { return m_id; }
    const std::string& get_name() const { return m_name; }

    View& get_view() { return m_view; }

private:

    uint64 m_id;
    std::string m_name;

    FrameGraphResourceType m_type;
    FrameGraphResourceFlags m_flags;

    std::reference_wrapper<View> m_view;

    std::vector<ResourceState> m_states;

    uint32 m_ref_count;
};

class FrameGraphResourceHandle {
friend class FrameGraphResourceManager;
public:

    FrameGraphResourceHandle() {

    }

    FrameGraphResourceHandle(const uint64 id) : m_id(id) {

    }

    bool operator==(const FrameGraphResourceHandle& rhs) const { return m_id == rhs.m_id; }
    
    static FrameGraphResourceHandle invalid() {
        return { std::numeric_limits<uint64>::max() };
    }

protected:

    uint64 get_id() const { return m_id; }

private:

    uint64 m_id;
};

class FrameGraphResourceManager {
public:

    FrameGraphResourceManager() : m_offset(0) {

    }

    FrameGraphResourceHandle create(const std::string& name, const FrameGraphResourceType type, View& view, const FrameGraphResourceFlags flags) {

        m_resources.emplace_back(m_offset, FrameGraphResourceType::Attachment, name, view, flags);
        m_resource_offsets[m_offset] = std::prev(m_resources.end());

        FrameGraphResourceHandle handle(m_offset);
        m_offset++;
        return handle;
    }

    FrameGraphResourceHandle find_handle_by_name(const std::string& name) {

        auto it = std::find_if(
            m_resources.begin(), 
            m_resources.end(),
            [&name](const FrameGraphResource& element) {
                return name == element.get_name();
            }
        );

        return it != m_resources.end() ? FrameGraphResourceHandle { it->get_id() } : FrameGraphResourceHandle::invalid();
    }

    FrameGraphResource& get_resource(const FrameGraphResourceHandle& handle) {
        return *m_resource_offsets[handle.get_id()];
    }

    std::list<FrameGraphResource>& get_resources() { return m_resources; }

private:

    uint64 m_offset;

    std::list<FrameGraphResource> m_resources;
    std::map<uint64, std::list<FrameGraphResource>::iterator> m_resource_offsets;
};

}