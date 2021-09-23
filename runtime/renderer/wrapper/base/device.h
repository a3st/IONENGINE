// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

/**
    @brief Graphics API Device class

    Device class for Graphics API. It's needed for create graphics api resources 
*/
class Device {
public:

    virtual ~Device() = default;

    virtual void wait(const CommandBufferType type, Fence* fence, const uint64 value) = 0;

    virtual void signal(const CommandBufferType type, Fence* fence, const uint64 value) = 0;

    virtual void execute_command_buffers(const std::vector<CommandBuffer*>& cmd_buffers) = 0;

    virtual std::unique_ptr<Buffer> create_buffer(const BufferType type, const BufferDesc& buffer_desc) = 0;

    virtual std::unique_ptr<Swapchain> create_swapchain(const SwapchainDesc& swapchain_desc) = 0;

    virtual const AdapterDesc& get_adapter_desc() const = 0;

    virtual std::unique_ptr<Memory> create_memory(const MemoryType memory_type, const usize size, const uint32 aligment, const BufferFlags buffer_flags) = 0;
    /*

    virtual std::unique_ptr<DescriptorSetLayout> create_descriptor_set_layout(const std::vector<DescriptorSetLayoutBinding>& bindings) = 0;
    
    virtual std::unique_ptr<RenderPass> create_render_pass(const RenderPassDesc& desc) = 0;

    virtual std::unique_ptr<Pipeline> create_graphics_pipeline(const GraphicsPipelineDesc& desc) = 0;
    

    virtual std::unique_ptr<DescriptorPool> create_descriptor_pool(const std::vector<DescriptorPoolSize>& sizes) = 0;
    
 
    virtual std::unique_ptr<Fence> create_fence(const uint64 value) = 0;

    virtual std::unique_ptr<Memory> allocate_memory(const MemoryType type, const usize size, const uint32 alignment, const ResourceFlags flags) = 0;
    

    virtual std::unique_ptr<CommandList> create_command_list(const CommandListType type) = 0;
    

    virtual std::unique_ptr<View> create_view(DescriptorPool& descriptor_pool, Resource& resource, const ViewDesc& desc) = 0;
   
 
    virtual std::unique_ptr<FrameBuffer> create_frame_buffer(const FrameBufferDesc& desc) = 0;*/
};

}