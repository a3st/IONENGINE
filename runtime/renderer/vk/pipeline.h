// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {
    
struct PipelineConfig final {

    std::vector<Shader>& shaders;

    PipelineConfig(std::vector<Shader>& shaders_) : shaders(shaders_) {
            
    }
};

class Pipeline final {
public:

    Pipeline(const PipelineConfig& pipeline_config) {

        VkGraphicsPipelineCreateInfo pipeline_info = {};
        pipeline_info.pInputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pipeline_info.pInputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        pipeline_config.pInputAssemblyState.primitiveRestartEnable = false;
        

        std::vector<VkPipelineShaderStageCreateInfo> stage_infos;
        stage_infos.resize(pipeline_config.shaders.size());
        
        pipeline_info.stageCount = stage_infos.size();
        pipeline_info.pStages = stage_infos.data();

        for(uint32 i = 0; i < pipeline_config.shaders.size(); ++i) {
            stage_infos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage_infos[i].module = pipeline_config.shaders[i].get_handle();
            switch(pipeline_config.shaders[i].get_type()) {
                case ShaderType::Vertex: stage_infos[i].stage = VK_SHADER_STAGE_VERTEX_BIT; break;
                case ShaderType::Fragment: stage_infos[i].stage = VK_SHADER_STAGE_FRAGMENT_BIT; break;
                default: break;
            }
        }
    }

    ~Pipeline() {
        
    }

private:

    VkPipelineLayout m_pipeline_layout_handle;
    VkPipeline m_pipeline_handle;
};


}