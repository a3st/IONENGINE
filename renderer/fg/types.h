// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::fg {

enum class TaskType {
    RenderPass,
    ComputePass
};

enum class RenderQueueType {
    Barrier,
    RenderPass,
    ComputePass
};

enum class BarrierType {
    Read,
    Write,
    Present
};

enum class ResourceType {
    Attachment,
    Buffer
};

enum class ResourceOp {
    Load,
    Clear
};

enum class ResourceFlags : uint32 {
    None = 1 << 0,
    Present = 1 << 1,
    DepthStencil = 1 << 2
};

ENUM_CLASS_BIT_FLAG_DECLARE(ResourceFlags)

struct AttachmentDesc {
    ResourceOp op;
    math::Fcolor clear_color;
};

struct BufferDesc {

};

class ResourceHandle {
friend class ResourceManager;
public:

    ResourceHandle() : m_id(std::numeric_limits<uint64>::max()) {

    }

    ResourceHandle(const uint64 id) : m_id(id) {

    }

    bool operator==(const ResourceHandle& rhs) const { return m_id == rhs.m_id; }
    
    static ResourceHandle null() {
        return { std::numeric_limits<uint64>::max() };
    }

private:

    uint64 m_id;
};

}