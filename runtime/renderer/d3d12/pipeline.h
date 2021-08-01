// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class PrimitiveTopologyType {
    Point = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT,
    Line = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE,
    Triangle = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE,
    Patch = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH
};

struct PipelineConfig {

    PrimitiveTopologyType primitive_topology;

    std::optional<std::reference_wrapper<Shader>> vertex_shader;
    std::optional<std::reference_wrapper<Shader>> frag_shader;
    std::optional<std::reference_wrapper<Shader>> geom_shader;
    std::optional<std::reference_wrapper<Shader>> hull_shader;
    std::optional<std::reference_wrapper<Shader>> domain_shader;
    std::optional<std::reference_wrapper<Shader>> compute_shader;

    PipelineConfig& set_primitive_topology(const PrimitiveTopologyType type) {
        primitive_topology = type;
        return *this; 
    }

    PipelineConfig& set_vertex_shader(Shader& shader) {
        vertex_shader = shader;
        return *this; 
    }

    PipelineConfig& set_frag_shader(Shader& shader) {
        frag_shader = shader;
        return *this;
    }

    PipelineConfig& set_geom_shader(Shader& shader) {
        geom_shader = shader;
        return *this; 
    }

    PipelineConfig& set_hull_shader(Shader& shader) {
        hull_shader = shader;
        return *this; 
    }

    PipelineConfig& set_domain_shader(Shader& shader) {
        domain_shader = shader;
        return *this; 
    }

    PipelineConfig& set_compute_shader(Shader& shader) {
        compute_shader = shader;
        return *this; 
    }
};

class Pipeline {
public:

    Pipeline(Device& device, const PipelineConfig& pipeline_config) {

    }

private:

    ComPtr<ID3D12PipelineState> m_pipeline_state;

};

}