// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

/**
    @brief Graphics API Device class

    Device class for Graphics API. It's needed for create graphics api resources 
*/
class Device {
public:

    virtual ~Device() = default;

    virtual void wait(const CommandListType command_list_type, Fence* fence, const uint64 value) = 0;

    virtual void signal(const CommandListType command_list_type, Fence* fence, const uint64 value) = 0;

    virtual void execute_command_buffers(const CommandListType command_list_type, const std::vector<CommandList*>& command_lists) = 0;

    virtual std::unique_ptr<Resource> create_resource(const ResourceType type, const ResourceDesc& resource_desc) = 0;

    virtual std::unique_ptr<Sampler> create_sampler(const SamplerDesc& sampler_desc) = 0;

    virtual std::unique_ptr<Fence> create_fence(const uint64 initial_value) = 0;

    virtual const AdapterDesc& get_adapter_desc() const = 0;

    virtual void present() = 0;

    virtual Resource* get_swapchain_resource(const uint32 resource_index) = 0;

    virtual uint32 get_swapchain_resource_index() const = 0;

    virtual std::unique_ptr<Memory> allocate_memory(const MemoryType memory_type, const usize size, const uint32 alignment, const ResourceFlags resource_flags) = 0;

    virtual std::unique_ptr<Pipeline> create_pipeline(const GraphicsPipelineDesc& pipeline_desc) = 0;

    virtual std::unique_ptr<Pipeline> create_pipeline(const ComputePipelineDesc& pipeline_desc) = 0;
};

}