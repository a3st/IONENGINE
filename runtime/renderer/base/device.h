// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Device {
public:

    virtual ~Device() = default;

    virtual std::shared_ptr<CommandQueue> get_command_queue(const CommandListType type) = 0;

    virtual std::unique_ptr<Swapchain> create_swapchain(void* hwnd, const uint32 width, const uint32 height, const uint32 buffer_count) = 0;
    
    virtual std::shared_ptr<Shader> create_shader(const std::vector<byte>& blob) = 0;
    virtual std::shared_ptr<DescriptorSetLayout> create_descriptor_set_layout(const std::vector<DescriptorSetLayoutBinding>& bindings) = 0;
    virtual std::shared_ptr<RenderPass> create_render_pass(const RenderPassDesc& desc) = 0;
    virtual std::shared_ptr<Pipeline> create_graphics_pipeline(const GraphicsPipelineDesc& desc) = 0;
};

}