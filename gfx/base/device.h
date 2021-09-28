// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::api {

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
    virtual CommandQueue& get_command_queue(const CommandListType list_type) = 0;

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
        @param shader_type type of shader
        @param shader_blob vector of bytes
        @return std::unique_ptr<Shader> unique pointer to shader
    */
    virtual std::unique_ptr<Shader> create_shader(const ShaderType shader_type, const std::vector<byte>& shader_blob) = 0;
    
    /**
        @brief Create a descriptor set layout from device
        @param bindings vector of bindings
        @return std::unique_ptr<DescriptorSetLayout> unique pointer to layout
    */
    virtual std::unique_ptr<DescriptorSetLayout> create_descriptor_set_layout(const std::vector<DescriptorSetLayoutBinding>& bindings) = 0;
    
    /**
        @brief Create a render pass from device
        @param render_pass_desc RenderPassDesc
        @return std::unique_ptr<RenderPass> unique pointer to render pass
    */
    virtual std::unique_ptr<RenderPass> create_render_pass(const RenderPassDesc& render_pass_desc) = 0;
    
    /**
        @brief Create a graphics pipeline from device
        @param pipeline_desc GraphicsPipelineDesc
        @return std::unique_ptr<Pipeline> unique pointer to pipeline
    */
    virtual std::unique_ptr<Pipeline> create_graphics_pipeline(const GraphicsPipelineDesc& pipeline_desc) = 0;
    
    /**
        @brief Create a resource from device
        @param resource_type resource type
        @param std::variant<ResourceDesc, SamplerDesc> ResourceDesc or SamplerDesc
        @return std::unique_ptr<Resource> unique pointer to resource 
    */
    virtual std::unique_ptr<Resource> create_resource(const ResourceType resource_type, const std::variant<ResourceDesc, SamplerDesc>& resource_desc) = 0;
    
    /**
        @brief Create a descriptor pool from device
        @param pool_sizes vector of pool sizes
        @return std::unique_ptr<DescriptorPool> unique pointer to descriptor pool
    */
    virtual std::unique_ptr<DescriptorPool> create_descriptor_pool(const std::vector<DescriptorPoolSize>& pool_sizes) = 0;
    
    /**
        @brief Create a fence from device
        @param initial_value initialization value
        @return std::unique_ptr<Fence> unique pointer to fence
    */
    virtual std::unique_ptr<Fence> create_fence(const uint64 initial_value) = 0;
    
    /**
        @brief Allocate new memory heap from device
        @param memory_type memory type
        @param size memory size in bytes
        @param alignment alignment
        @param flags memory flags
        @return std::unique_ptr<Memory> unique pointer to memory
    */
    virtual std::unique_ptr<Memory> allocate_memory(const MemoryType memory_type, const usize size, const uint32 alignment, const ResourceFlags flags) = 0;
    
    /**
        @brief Create a command list from device
        @param list_type CommandList type
        @return std::unique_ptr<CommandList> unique pointer to command list 
    */
    virtual std::unique_ptr<CommandList> create_command_list(const CommandListType list_type) = 0;
    
    /**
        @brief Create a view from device
        @param descriptor_pool reference to DescriptorPool
        @param resource reference to Resource
        @param view_desc ViewDesc
        @return std::unique_ptr<View> unique pointer to view
    */
    virtual std::unique_ptr<View> create_view(DescriptorPool& descriptor_pool, Resource& resource, const ViewDesc& view_desc) = 0;
   
    /**
        @brief Create a frame buffer from device
        @param frame_buffer_desc FrameBufferDesc
        @return std::unique_ptr<FrameBuffer> unique pointer to frame buffer
    */
    virtual std::unique_ptr<FrameBuffer> create_frame_buffer(const FrameBufferDesc& frame_buffer_desc) = 0;
};

}