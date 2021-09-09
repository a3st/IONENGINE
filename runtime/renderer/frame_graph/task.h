// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class FrameGraphTaskType {
    ResourceBarrier,
    RenderPass
};

class FrameGraphTask {
public:

    FrameGraphTask(const ResourceBarrierDesc& desc) 
        : m_type(FrameGraphTaskType::ResourceBarrier), m_task(desc) {

    }

    FrameGraphTaskType get_type() const { return m_type; }

    std::variant<
        ResourceBarrierDesc,
        std::reference_wrapper<FrameGraphRenderPass>
    > get_task() const { return m_task; }

private:

    FrameGraphTaskType m_type;

    std::variant<
        ResourceBarrierDesc,
        std::reference_wrapper<FrameGraphRenderPass>
    > m_task;
};

}