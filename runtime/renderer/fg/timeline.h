// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::fg {

struct BarrierTimeline {
    std::vector<api::ResourceBarrierDesc> data;
};

struct RenderPassTimeline {
    std::vector<api::RenderPassColorDesc> color_descs;
    api::RenderPassDepthStencilDesc depth_stencil_desc;
    uint32 sample_count;
    std::vector<std::reference_wrapper<api::View>> color_views;
    std::optional<std::reference_wrapper<api::View>> depth_stencil_view;
};

class Timeline {
public:

    Timeline(const TimelineType type, const std::variant<BarrierTimeline, RenderPassTimeline>& timeline) 
        : m_type(type), m_timeline(timeline) {

    }

    TimelineType get_type() const { return m_type; }

    const std::variant<
        BarrierTimeline,
        RenderPassTimeline
    >& get_timeline() const { return m_timeline; }

private:

    TimelineType m_type;

    std::variant<
        BarrierTimeline,
        RenderPassTimeline
    > m_timeline;
};

}