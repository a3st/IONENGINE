// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Device {
public:

    virtual ~Device() = default;

    virtual CommandQueue& get_command_queue(const CommandListType type) = 0;

    virtual std::unique_ptr<Swapchain> create_swapchain(void* hwnd, const uint32 width, const uint32 height, const uint32 buffer_count) = 0;
    virtual std::unique_ptr<Shader> create_shader(const std::vector<byte>& blob) = 0;
    virtual std::unique_ptr<DescriptorSetLayout> create_descriptor_set_layout(const std::vector<DescriptorSetLayoutBinding>& bindings) = 0;
    virtual std::unique_ptr<RenderPass> create_render_pass(const RenderPassDesc& desc) = 0;
    virtual std::unique_ptr<Pipeline> create_graphics_pipeline(const GraphicsPipelineDesc& desc) = 0;
    virtual std::unique_ptr<Resource> create_buffer(const ResourceFlags flags, const usize buffer_size) = 0;
    virtual std::unique_ptr<DescriptorPool> create_descriptor_pool(const std::vector<DescriptorPoolSize>& sizes) = 0;

    virtual std::shared_ptr<Memory> allocate_memory(const MemoryType memory_type, const usize size, const uint32 align, const ResourceFlags memory_flags) = 0;
};

}