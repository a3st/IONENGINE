// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::fg {

struct BarrierQueueData {
    std::vector<api::ResourceBarrierDesc> data;
};

struct RenderPassQueueData {
    std::vector<api::RenderPassColorDesc> color_descs;
    api::RenderPassDepthStencilDesc depth_stencil_desc;
    uint32 sample_count;
    std::vector<std::reference_wrapper<api::View>> color_views;
    std::optional<std::reference_wrapper<api::View>> depth_stencil_view;
    api::ClearValueDesc clear_value_desc;
    std::function<void(RenderPassContext&)> exec_func;
};

class RenderQueue {
public:

    RenderQueue(const RenderQueueType type, const std::variant<BarrierQueueData, RenderPassQueueData>& data) 
        : m_type(type), m_data(data) {

    }

    RenderQueueType get_type() const { return m_type; }

    const std::variant<
        BarrierQueueData,
        RenderPassQueueData
    >& get_data() const { return m_data; }

private:

    RenderQueueType m_type;

    std::variant<
        BarrierQueueData,
        RenderPassQueueData
    > m_data;
};

}