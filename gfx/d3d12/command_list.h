// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DCommandList : public CommandList {
public:

    D3DCommandList(ID3D12Device4* d3d12_device, const CommandListType command_list_type) : m_d3d12_device(d3d12_device) {

        assert(d3d12_device && "pointer to d3d12_device is null");
        
        THROW_IF_FAILED(d3d12_device->CreateCommandAllocator(
                gfx_to_d3d12_command_list_type(command_list_type), 
                __uuidof(ID3D12CommandAllocator), m_d3d12_command_allocator.put_void()
            )
        );

        THROW_IF_FAILED(d3d12_device->CreateCommandList(
                0, 
                gfx_to_d3d12_command_list_type(command_list_type), 
                m_d3d12_command_allocator.get(), 
                nullptr, 
                __uuidof(ID3D12GraphicsCommandList), m_d3d12_command_list.put_void()
            )
        );
        THROW_IF_FAILED(m_d3d12_command_list->Close());
    }

    void bind_pipeline(Pipeline* pipeline) override {

        assert(pipeline && "pointer to pipeline is null");

        m_current_pipeline = static_cast<D3DPipeline*>(pipeline);

        D3DBindingSetLayout* d3d_layout = static_cast<D3DBindingSetLayout*>(m_current_pipeline->get_binding_set_layout());

        if(m_current_pipeline->get_type() == PipelineType::Graphics) {

            m_d3d12_command_list->SetGraphicsRootSignature(d3d_layout->get_d3d12_root_signature());
            m_d3d12_command_list->SetPipelineState(m_current_pipeline->get_d3d12_pipeline_state());
            m_d3d12_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        } else {

            m_d3d12_command_list->SetComputeRootSignature(d3d_layout->get_d3d12_root_signature());
        }
    }
    
    void set_viewport(const int32 x, const int32 y, const uint32 width, const uint32 height) override {

        D3D12_VIEWPORT viewport{};
        viewport.TopLeftX = static_cast<float>(x);
        viewport.TopLeftY = static_cast<float>(y);
        viewport.Width = static_cast<float>(width);
        viewport.Height = static_cast<float>(height);
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;

        m_d3d12_command_list->RSSetViewports(1, &viewport);
    }

    void set_scissor_rect(int32 left, int32 top, int32 right, int32 bottom) override {

        D3D12_RECT rect{};
        rect.top = static_cast<int32>(top);
        rect.bottom = static_cast<int32>(bottom);
        rect.left = static_cast<int32>(left);
        rect.right = static_cast<int32>(right);

        m_d3d12_command_list->RSSetScissorRects(1, &rect);
    }

    void resource_barriers(const std::vector<ResourceBarrierDesc>& barriers) override {
        
        std::vector<D3D12_RESOURCE_BARRIER> d3d12_barriers;
        for(auto& barrier : barriers) {

            D3D12_RESOURCE_BARRIER resource_barrier{};
            resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            resource_barrier.Transition.pResource = static_cast<D3DResource*>(barrier.resource)->get_d3d12_resource();
            resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            resource_barrier.Transition.StateAfter = gfx_to_d3d12_resource_state(barrier.after);
            resource_barrier.Transition.StateBefore = gfx_to_d3d12_resource_state(barrier.before);

            d3d12_barriers.emplace_back(resource_barrier);
        }

        if(!d3d12_barriers.empty()) {
            m_d3d12_command_list->ResourceBarrier(static_cast<uint32>(d3d12_barriers.size()), d3d12_barriers.data());
        }
    }

    void begin_render_pass(RenderPass* render_pass, FrameBuffer* frame_buffer, const ClearValueDesc& clear_value_desc) override {

        assert(render_pass && "pointer to render_pass is null");
        assert(frame_buffer && "pointer to frame_buffer is null");

        auto& render_pass_desc = static_cast<D3DRenderPass*>(render_pass)->get_desc();

        auto& colors_views = static_cast<D3DFrameBuffer*>(frame_buffer)->get_desc().colors;
        auto depth_stencil_view = static_cast<D3DFrameBuffer*>(frame_buffer)->get_desc().depth_stencil;

        std::vector<D3D12_RENDER_PASS_RENDER_TARGET_DESC> render_pass_render_targets;
        for(uint32 i = 0; i < colors_views.size(); ++i) {

            D3D12_RENDER_PASS_BEGINNING_ACCESS begin{};
            begin.Type = gfx_to_d3d12_render_pass_begin_type(render_pass_desc.colors[i].load_op);
            
            begin.Clear.ClearValue.Format = gfx_to_dxgi_format(render_pass_desc.colors[i].format);
            std::memcpy(&begin.Clear.ClearValue.Color[0], &clear_value_desc.colors[0], sizeof(ClearValueColor));

            D3D12_RENDER_PASS_ENDING_ACCESS end{};
            end.Type = gfx_to_d3d12_render_pass_end_type(render_pass_desc.colors[i].store_op);

            D3D12_RENDER_PASS_RENDER_TARGET_DESC render_pass_render_target;

            auto d3d_view = static_cast<D3DView*>(colors_views[i]);

            D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = {
                d3d_view->get_descriptor_ptr().heap->d3d12_heap->GetCPUDescriptorHandleForHeapStart().ptr + 
                    d3d_view->get_descriptor_ptr().offset * m_d3d12_device->GetDescriptorHandleIncrementSize(gfx_to_d3d12_descriptor_heap_type(d3d_view->get_type()))
            };

            render_pass_render_target.cpuDescriptor = cpu_handle;
            render_pass_render_target.BeginningAccess = begin;
            render_pass_render_target.EndingAccess = end;

            render_pass_render_targets.emplace_back(render_pass_render_target);      
        }

        D3D12_RENDER_PASS_DEPTH_STENCIL_DESC render_pass_depth_stencil;
        if(depth_stencil_view) {

            D3D12_RENDER_PASS_BEGINNING_ACCESS depth_begin{};
            depth_begin.Type = gfx_to_d3d12_render_pass_begin_type(render_pass_desc.depth_stencil.depth_load_op);
            depth_begin.Clear.ClearValue.Format = static_cast<DXGI_FORMAT>(render_pass_desc.depth_stencil.format);
            depth_begin.Clear.ClearValue.DepthStencil.Depth = clear_value_desc.depth;

            D3D12_RENDER_PASS_ENDING_ACCESS depth_end{};
            depth_end.Type = gfx_to_d3d12_render_pass_end_type(render_pass_desc.depth_stencil.depth_store_op);

            D3D12_RENDER_PASS_BEGINNING_ACCESS stencil_begin{};
            stencil_begin.Type = gfx_to_d3d12_render_pass_begin_type(render_pass_desc.depth_stencil.stencil_load_op);

            stencil_begin.Clear.ClearValue.Format = static_cast<DXGI_FORMAT>(render_pass_desc.depth_stencil.format);
            stencil_begin.Clear.ClearValue.DepthStencil.Stencil = clear_value_desc.stencil;

            D3D12_RENDER_PASS_ENDING_ACCESS stencil_end{};
            stencil_end.Type = gfx_to_d3d12_render_pass_end_type(render_pass_desc.depth_stencil.stencil_store_op);

            auto d3d_view = static_cast<D3DView*>(depth_stencil_view);

            D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = {
                d3d_view->get_descriptor_ptr().heap->d3d12_heap->GetCPUDescriptorHandleForHeapStart().ptr + 
                    d3d_view->get_descriptor_ptr().offset * m_d3d12_device->GetDescriptorHandleIncrementSize(gfx_to_d3d12_descriptor_heap_type(d3d_view->get_type()))
            };

            render_pass_depth_stencil.cpuDescriptor = cpu_handle;
            render_pass_depth_stencil.DepthBeginningAccess = depth_begin;
            render_pass_depth_stencil.DepthEndingAccess = depth_end;
            render_pass_depth_stencil.StencilBeginningAccess = stencil_begin;
            render_pass_depth_stencil.StencilEndingAccess = stencil_end;

            m_d3d12_command_list->BeginRenderPass(static_cast<uint32>(render_pass_render_targets.size()), render_pass_render_targets.data(), &render_pass_depth_stencil, D3D12_RENDER_PASS_FLAG_NONE);
        } else {
            m_d3d12_command_list->BeginRenderPass(static_cast<uint32>(render_pass_render_targets.size()), render_pass_render_targets.data(), nullptr, D3D12_RENDER_PASS_FLAG_NONE);
        }
    }

    void end_render_pass() override {
        m_d3d12_command_list->EndRenderPass();
    }

    void draw_indexed(const uint32 index_count, const uint32 instance_count, const uint32 first_index, const int32 vertex_offset, const uint32 first_instance) override {
        m_d3d12_command_list->DrawIndexedInstanced(index_count, instance_count, first_index, vertex_offset, first_instance);
    }

    void close() override {
        m_d3d12_command_list->Close();
    }

    void reset() override {
        m_d3d12_command_list->Reset(m_d3d12_command_allocator.get(), nullptr);
    }

    void set_index_buffer(Resource* resource, const Format format) override {

        assert(resource && "pointer to resource is null");

        auto d3d_resource = static_cast<D3DResource*>(resource);
        auto& d3d12_resource_desc = std::get<D3D12_RESOURCE_DESC>(d3d_resource->get_d3d12_desc());

        D3D12_INDEX_BUFFER_VIEW index_view{};
        index_view.Format = gfx_to_dxgi_format(format);
        index_view.BufferLocation = d3d_resource->get_d3d12_resource()->GetGPUVirtualAddress();
        index_view.SizeInBytes = static_cast<uint32>(d3d12_resource_desc.Width);
        m_d3d12_command_list->IASetIndexBuffer(&index_view);
    }

    void set_vertex_buffer(const uint32 slot, Resource* resource, const uint32 stride) override {

        assert(resource && "pointer to resource is null");

        auto d3d_resource = static_cast<D3DResource*>(resource);
        auto& d3d12_resource_desc = std::get<D3D12_RESOURCE_DESC>(d3d_resource->get_d3d12_desc());

        D3D12_VERTEX_BUFFER_VIEW vertex_view{};
        vertex_view.StrideInBytes = stride;
        vertex_view.BufferLocation = d3d_resource->get_d3d12_resource()->GetGPUVirtualAddress();
        vertex_view.SizeInBytes = static_cast<uint32>(d3d12_resource_desc.Width);
        m_d3d12_command_list->IASetVertexBuffers(slot, 1, &vertex_view);
    }

    void copy_buffer(Resource* src_resource, Resource* dst_resource, const BufferCopyRegion& region) override {

        auto d3d_src_resource = static_cast<D3DResource*>(src_resource);
        auto d3d_dst_resource = static_cast<D3DResource*>(dst_resource);

        m_d3d12_command_list->CopyBufferRegion(d3d_dst_resource->get_d3d12_resource(), region.dst_offset, d3d_src_resource->get_d3d12_resource(), region.src_offset, region.size);
    }

    ID3D12CommandAllocator* get_d3d12_command_allocator() { return m_d3d12_command_allocator.get(); }

    ID3D12GraphicsCommandList4* get_d3d12_command_list() { return m_d3d12_command_list.get(); }

private:

    ID3D12Device4* m_d3d12_device;

    winrt::com_ptr<ID3D12CommandAllocator> m_d3d12_command_allocator;
    winrt::com_ptr<ID3D12GraphicsCommandList4> m_d3d12_command_list;

    D3DPipeline* m_current_pipeline;
};

}