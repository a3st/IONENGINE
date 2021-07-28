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

    Pipeline(const Device& device, const PipelineConfig& pipeline_config) : m_device(device) {

        VkGraphicsPipelineCreateInfo pipeline_info = {};
        

        std::vector<VkPipelineShaderStageCreateInfo> stage_infos;
        stage_infos.resize(pipeline_config.shaders.size());
        
        pipeline_info.stageCount = static_cast<uint32>(stage_infos.size());
        pipeline_info.pStages = stage_infos.data();

        for(uint32 i = 0; i < pipeline_config.shaders.size(); ++i) {
            stage_infos[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            stage_infos[i].module = pipeline_config.shaders[i].get_handle();
            stage_infos[i].stage = static_cast<VkShaderStageFlagBits>(pipeline_config.shaders[i].get_type());
            stage_infos[i].pName = "main";
        }
    }

    ~Pipeline() {
        //vkDestroyPipelineLayout(m_device.get_handle(), m_pipeline_layout_handle, nullptr);
        //vkDestroyPipeline(m_device.get_handle(), m_pipeline_handle, nullptr);
    }

private:

    const Device& m_device;

    VkPipelineLayout m_pipeline_layout_handle;
    VkPipeline m_pipeline_handle;
};

}