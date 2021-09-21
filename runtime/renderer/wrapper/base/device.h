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

    /**
        @brief Get the command queue from device
        @param list_type CommandList type
        @return CommandQueue& reference to CommandQueue object
    */
    virtual CommandQueue& get_command_queue(const CommandListType type) = 0;

    /**
        @brief Create a swapchain from device
        @param hwnd pointer to HWND handle
        @param width swapchain width
        @param height swapchain height
        @param buffer_count swapchain buffer count
        @return std::unique_ptr<Swapchain> unique pointer to swapchain
    */
    virtual std::unique_ptr<Swapchain> create_swapchain(void* hwnd, const uint32 width, const uint32 height, const uint32 buffer_count) = 0;
    
    /**
        @brief Create a shader from device
        @param type type of shader
        @param blob vector of bytes
        @return std::unique_ptr<Shader> unique pointer to shader
    */
    virtual std::unique_ptr<Shader> create_shader(const ShaderType type, const std::vector<byte>& blob) = 0;
    
    /**
        @brief Create a descriptor set layout from device
        @param bindings vector of bindings
        @return std::unique_ptr<DescriptorSetLayout> unique pointer to layout
    */
    virtual std::unique_ptr<DescriptorSetLayout> create_descriptor_set_layout(const std::vector<DescriptorSetLayoutBinding>& bindings) = 0;
    
    /**
        @brief Create a render pass from device
        @param desc RenderPassDesc
        @return std::unique_ptr<RenderPass> unique pointer to render pass
    */
    virtual std::unique_ptr<RenderPass> create_render_pass(const RenderPassDesc& desc) = 0;
    
    /**
        @brief Create a graphics pipeline from device
        @param desc GraphicsPipelineDesc
        @return std::unique_ptr<Pipeline> unique pointer to pipeline
    */
    virtual std::unique_ptr<Pipeline> create_graphics_pipeline(const GraphicsPipelineDesc& desc) = 0;
    
    /**
        @brief Create a resource from device
        @param type resource type
        @param desc ResourceDesc or SamplerDesc
        @return std::unique_ptr<Resource> unique pointer to resource 
    */
    virtual std::unique_ptr<Resource> create_resource(const ResourceType type, const std::variant<ResourceDesc, SamplerDesc>& desc) = 0;
    
    /**
        @brief Create a descriptor pool from device
        @param sizes vector of pool sizes
        @return std::unique_ptr<DescriptorPool> unique pointer to descriptor pool
    */
    virtual std::unique_ptr<DescriptorPool> create_descriptor_pool(const std::vector<DescriptorPoolSize>& sizes) = 0;
    
    /**
        @brief Create a fence from device
        @param value initialization value
        @return std::unique_ptr<Fence> unique pointer to fence
    */
    virtual std::unique_ptr<Fence> create_fence(const uint64 value) = 0;
    
    /**
        @brief Allocate new memory heap from device
        @param type memory type
        @param size memory size in bytes
        @param alignment alignment
        @param flags memory flags
        @return std::unique_ptr<Memory> unique pointer to memory
    */
    virtual std::unique_ptr<Memory> allocate_memory(const MemoryType type, const usize size, const uint32 alignment, const ResourceFlags flags) = 0;
    
    /**
        @brief Create a command list from device
        @param type CommandList type
        @return std::unique_ptr<CommandList> unique pointer to command list 
    */
    virtual std::unique_ptr<CommandList> create_command_list(const CommandListType type) = 0;
    
    /**
        @brief Create a view from device
        @param descriptor_pool reference to DescriptorPool
        @param resource reference to Resource
        @param desc ViewDesc
        @return std::unique_ptr<View> unique pointer to view
    */
    virtual std::unique_ptr<View> create_view(DescriptorPool& descriptor_pool, Resource& resource, const ViewDesc& desc) = 0;
   
    /**
        @brief Create a frame buffer from device
        @param desc FrameBufferDesc
        @return std::unique_ptr<FrameBuffer> unique pointer to frame buffer
    */
    virtual std::unique_ptr<FrameBuffer> create_frame_buffer(const FrameBufferDesc& desc) = 0;
};

}