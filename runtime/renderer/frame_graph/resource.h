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

struct FrameGraphResourceDesc {
    FrameGraphResourceType type;

    uint64 id;
    std::string name;

    std::reference_wrapper<View> view;

    std::vector<std::reference_wrapper<FrameGraphRenderPassDesc>> inputs;
    std::vector<std::reference_wrapper<FrameGraphRenderPassDesc>> outputs;
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

        m_resource_descs.emplace_back( FrameGraphResourceDesc { FrameGraphResourceType::Attachment, m_offset, name, view } );
        m_resource_desc_offsets[m_offset] = std::prev(m_resource_descs.end());

        FrameGraphResourceHandle handle(m_offset);
        m_offset++;
        return handle;
    }

    FrameGraphResourceHandle find_handle_by_name(const std::string& name) {

        auto it = std::find_if(
            m_resource_descs.begin(), 
            m_resource_descs.end(),
            [&name](const FrameGraphResourceDesc& element) {
                return name == element.name;
            }
        );

        return it != m_resource_descs.end() ? FrameGraphResourceHandle { it->id } : FrameGraphResourceHandle::invalid();
    }

    FrameGraphResourceDesc& get_resource_desc(const FrameGraphResourceHandle& handle) {
        return *m_resource_desc_offsets[handle.get_id()];
    }

private:

    uint64 m_offset;

    std::list<FrameGraphResourceDesc> m_resource_descs;
    std::map<uint64, std::list<FrameGraphResourceDesc>::iterator> m_resource_desc_offsets;
};

}