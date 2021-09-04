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

class FrameGraphResource {
friend class FrameGraphResourceManager;
public:

    FrameGraphResource(const uint64 id, const std::string& name, const FrameGraphResourceType type, View& view) : m_id(id), m_name(name), m_type(type), m_view(view) {

    }

private:

    FrameGraphResourceType m_type;

    uint64 m_id;
    std::string m_name;

    std::reference_wrapper<View> m_view;
};

class FrameGraphResourceHandle {
friend class FrameGraphResourceManager;
public:

    FrameGraphResourceHandle() {

    }

    FrameGraphResourceHandle(const uint64 id) : m_id(id) {

    }

    bool operator==(const FrameGraphResourceHandle& rhs) const { return m_id == rhs.m_id; }
    
    static FrameGraphResourceHandle null() {
        return { std::numeric_limits<uint64>::max() };
    }

    uint64 get_id() const { return m_id; }

private:

    uint64 m_id;
};

class FrameGraphResourceManager {
public:

    FrameGraphResourceManager() : m_offset(0) {

    }

    FrameGraphResourceHandle create(const std::string& name, const AttachmentDesc& desc, View& view) {

        m_resources.emplace_back(m_offset, name, FrameGraphResourceType::Attachment, view);
        m_resource_handles[m_offset] = std::prev(m_resources.end());

        FrameGraphResourceHandle handle(m_offset);
        m_offset++;
        return handle;
    }

    FrameGraphResourceHandle get_by_name(const std::string& name) {

        auto it = std::find_if(
            m_resources.begin(), m_resources.end(),
            [&name](const FrameGraphResource& element) {
                return name == element.m_name;
            }
        );

        return it != m_resources.end() ? FrameGraphResourceHandle { it->m_id } : FrameGraphResourceHandle { std::numeric_limits<uint64>::max() };
    }

private:

    uint64 m_offset;

    std::list<FrameGraphResource> m_resources;
    std::map<uint64, std::list<FrameGraphResource>::iterator> m_resource_handles;
};

}