// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class FrameGraphResourceType {
    Attachment,
    Buffer
};

struct AttachmentDesc {
    Format format;
    uint32 width;
    uint32 height;

    bool operator<(const AttachmentDesc& rhs) const {
        return std::tie(format, width, height) < std::tie(rhs.format, rhs.width, rhs.height);
    }
};

struct BufferDesc {
    uint64 size;

    bool operator<(const BufferDesc& rhs) const {
        return std::tie(size) < std::tie(rhs.size);
    }
};

class FrameGraphResource {
friend class FrameGraphResourceManager;
public:

    FrameGraphResource(const uint64 id, const FrameGraphResourceType type, const std::string& name, View& view) :
        m_id(id), m_type(type), m_name(name), m_view(view) {

    }

    void set_presentable() { m_presentable = true; }

    bool presentable() const { return m_presentable; }

    void acquire() {

        switch(m_type) {
            case FrameGraphResourceType::Attachment: {
                m_states.emplace_back(ResourceState::RenderTarget);
                break;
            }
        }
    }

    void release() {

        if(!m_presentable) {
            switch(m_type) {
                case FrameGraphResourceType::Attachment: {
                    m_states.emplace_back(ResourceState::PixelShaderResource);
                    break;
                }
            }
        } else {
            m_states.emplace_back(ResourceState::Present);
        }
    }

    void clear() {

        m_states.clear();

        if(!m_presentable) {
            switch(m_type) {
                case FrameGraphResourceType::Attachment: {
                    m_states.emplace_back(ResourceState::PixelShaderResource);
                    break;
                }
            }
        } else {
            m_states.emplace_back(ResourceState::Present);
        }
    }

    ResourceState get_before_state() const {
        return m_states[m_states.size() - 2];
    }

    ResourceState get_after_state() const {
        return m_states.back();
    }

    void print_test() const {

        std::cout << "resource id: " << get_id() << " states: " << std::endl;
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

    const std::variant<
        AttachmentDesc,
        BufferDesc
    >& get_desc() const { return m_desc; }

private:

    uint64 m_id;
    std::string m_name;

    FrameGraphResourceType m_type;
    std::variant<
        AttachmentDesc,
        BufferDesc
    > m_desc;

    std::reference_wrapper<View> m_view;

    std::vector<ResourceState> m_states;
    bool m_presentable;
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

    FrameGraphResourceHandle create(const std::string& name, const AttachmentDesc& desc, View& view) {

        m_resources.emplace_back(m_offset, FrameGraphResourceType::Attachment, name, view);
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