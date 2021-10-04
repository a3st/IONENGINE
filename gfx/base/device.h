// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

/**
    @brief Graphics API Device class

    Device class for Graphics API. It's needed for create graphics api resources 
*/

template<class B = backend::base>
class Device {
public:

    void wait(const CommandListType command_list_type, Fence<B>* fence, const uint64 value);

    void signal(const CommandListType command_list_type, Fence<B>* fence, const uint64 value);

    //void execute_command_lists(const CommandListType command_list_type, const std::vector<CommandList*>& command_lists);

    [[nodiscard]] std::unique_ptr<Resource<B>> create_resource(const ResourceType resource_type, const MemoryType memory_type, const ResourceDesc& resource_desc);

    [[nodiscard]] std::unique_ptr<View<B>> create_view(const ViewType view_type, Resource<B>* resource, const ViewDesc& view_desc);

    const AdapterDesc& get_adapter_desc() const;


    void present();

    [[nodiscard]] std::unique_ptr<Resource<B>> create_swapchain_resource(const uint32 buffer_index);

/*

    

    virtual 
    virtual [[nodiscard]] std::unique_ptr<Sampler> create_sampler(const SamplerDesc& sampler_desc) = 0;

    virtual [[nodiscard]] std::unique_ptr<Fence> create_fence(const uint64 initial_value) = 0;

    

    virtual  = 0;

    virtual  = 0;

    virtual uint32 get_swapchain_buffer_index() const = 0;

    virtual uint32 get_swapchain_buffer_size() const = 0;

    virtual [[nodiscard]] std::unique_ptr<Pipeline> create_pipeline(const GraphicsPipelineDesc& pipeline_desc) = 0;

    virtual [[nodiscard]] std::unique_ptr<Pipeline> create_pipeline(const ComputePipelineDesc& pipeline_desc) = 0;

    virtual [[nodiscard]] std::unique_ptr<View> create_view(const ViewType view_type, Resource* resource, const ViewDesc& view_desc) = 0;

    virtual [[nodiscard]] std::unique_ptr<View> create_view(Sampler* sampler) = 0;

    virtual [[nodiscard]] std::unique_ptr<BindingSet> create_binding_set(BindingSetLayout* layout) = 0;

    virtual [[nodiscard]] std::unique_ptr<BindingSetLayout> create_binding_set_layout(const std::vector<BindingSetBinding>& bindings) = 0;

    virtual [[nodiscard]] std::unique_ptr<RenderPass> create_render_pass(const RenderPassDesc& render_pass_desc) = 0;

    virtual [[nodiscard]] std::unique_ptr<FrameBuffer> create_frame_buffer(const FrameBufferDesc frame_buffer_desc) = 0;

    virtual [[nodiscard]] std::unique_ptr<CommandList> create_command_list(const CommandListType command_list_type) = 0;*/


};

}