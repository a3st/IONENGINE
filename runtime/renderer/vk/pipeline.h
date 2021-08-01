// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {
    
enum class PrimitiveTopologyType {
    Point = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    Line = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    Triangle = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    Patch = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST
};

enum class VertexInputRateType {
    Vertex = VK_VERTEX_INPUT_RATE_VERTEX,
    Instance = VK_VERTEX_INPUT_RATE_INSTANCE
};

struct VertexInputDesc {

    uint32 location;
    uint32 binding;
    FormatType format;
    uint32 offset;
    VertexInputRateType input_rate;
};

struct PipelineConfig {

    PrimitiveTopologyType primitive_topology;
    std::optional<std::reference_wrapper<std::vector<VertexInputDesc>>> vertex_input_desc;

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

    PipelineConfig& set_vertex_input_desc(std::vector<VertexInputDesc>& desc) {
        vertex_input_desc = desc;
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

        vk::GraphicsPipelineCreateInfo pipeline_info{};

        vk::PipelineVertexInputStateCreateInfo vertex_input_info{};

        for(uint32 i = 0; i < pipeline_config.vertex_input_desc.value().get().size(); ++i) {
            
        }
            

        //pipeline_info
          //  .setPVertexInputState()
        
        
        /*std::vector<VkPipelineShaderStageCreateInfo> stage_infos;
        stage_infos.resize(pipeline_config.shaders.size());
        
        pipeline_info.stageCount = static_cast<uint32>(stage_infos.size());
        pipeline_info.pStages = stage_infos.data();

        for(uint32 i = 0; i < pipeline_config.shaders.size(); ++i) {
            stage_infos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage_infos[i].module = pipeline_config.shaders[i].get_handle();
            stage_infos[i].stage = static_cast<VkShaderStageFlagBits>(pipeline_config.shaders[i].get_type());
            stage_infos[i].pName = "main";
        }*/
    }

private:

    VkPipeline m_pipeline;
};

}