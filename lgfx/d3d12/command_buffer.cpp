// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "command_buffer.h"
#include "device.h"
#include "texture.h"
#include "texture_view.h"
#include "render_pass.h"
#include "frame_buffer.h"
#include "pipeline.h"
#include "descriptor_set.h"
#include "buffer_view.h"
#include "buffer.h"
#include "descriptor_layout.h"
#include "conversion.h"

using namespace lgfx;

CommandBuffer::CommandBuffer(Device* device, const CommandBufferType type) : device_(device) {

    D3D12_COMMAND_LIST_TYPE list_type = ToD3D12CommandListType(type);
    THROW_IF_FAILED(device->device_->CreateCommandAllocator(list_type, __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(allocator_.GetAddressOf())));
    THROW_IF_FAILED(device->device_->CreateCommandList(0, list_type, allocator_.Get(), nullptr, __uuidof(ID3D12GraphicsCommandList4), reinterpret_cast<void**>(list_.GetAddressOf())));
    THROW_IF_FAILED(list_->Close());
}

void CommandBuffer::BindPipeline(Pipeline* pipeline) {

    list_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    list_->SetPipelineState(pipeline->pipeline_state_.Get());
}

void CommandBuffer::BindDescriptorSet(DescriptorSet* set) {

    list_->SetGraphicsRootSignature(set->layout_->root_signature_.Get());
    
    std::array<ID3D12DescriptorHeap*, 2> heaps = { set->srv_pool_.GetHeaps()[0]->heap_.Get(), set->sampler_pool_.GetHeaps()[0]->heap_.Get() }; 
	list_->SetDescriptorHeaps(static_cast<uint32_t>(heaps.size()), heaps.data());

    for(uint32_t i = 0; i < static_cast<uint32_t>(set->update_descriptors_.size()); ++i) {
        if(set->update_descriptors_[i].type == DescriptorType::kShaderResource || set->update_descriptors_[i].type == DescriptorType::kConstantBuffer || set->update_descriptors_[i].type == DescriptorType::kUnorderedAccess) {
            D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = { set->update_descriptors_[i].alloc_info.heap->heap_->GetGPUDescriptorHandleForHeapStart().ptr + set->update_descriptors_[i].alloc_info.offset * device_->srv_descriptor_offset_ };
            list_->SetGraphicsRootDescriptorTable(set->update_descriptors_[i].index, gpu_handle);
        } else {
            D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = { set->update_descriptors_[i].alloc_info.heap->heap_->GetGPUDescriptorHandleForHeapStart().ptr + set->update_descriptors_[i].alloc_info.offset * device_->sampler_descriptor_offset_ };
            list_->SetGraphicsRootDescriptorTable(set->update_descriptors_[i].index, gpu_handle);
        }
    }

    set->update_descriptors_.clear();
}

void CommandBuffer::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height) {

    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = static_cast<float>(x);
    viewport.TopLeftY = static_cast<float>(y);
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = D3D12_MIN_DEPTH;
    viewport.MaxDepth = D3D12_MAX_DEPTH;

    list_->RSSetViewports(1, &viewport);
}

void CommandBuffer::SetScissorRect(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom) {

    D3D12_RECT rect{};
    rect.top = static_cast<LONG>(top);
    rect.bottom = static_cast<LONG>(bottom);
    rect.left = static_cast<LONG>(left);
    rect.right = static_cast<LONG>(right);

    list_->RSSetScissorRects(1, &rect);
}

void CommandBuffer::BeginRenderPass(RenderPass* render_pass, FrameBuffer* frame_buffer, const std::span<ClearValueColor>& colors, float depth, uint8_t stencil) {

    for(size_t i : std::views::iota(0u, render_pass->render_pass_target_descs_.size())) {

        std::memcpy(render_pass->render_pass_target_descs_[i].BeginningAccess.Clear.ClearValue.Color, &colors[i], sizeof(ClearValueColor));

        DescriptorAllocInfo alloc_info = frame_buffer->colors_[i]->alloc_info_;
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { alloc_info.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + alloc_info.offset * device_->rtv_descriptor_offset_ };

        render_pass->render_pass_target_descs_[i].cpuDescriptor = cpu_handle;
    }

    if(frame_buffer->depth_stencil_) {
        {
            render_pass->render_pass_depth_stencil_desc_.DepthBeginningAccess.Clear.ClearValue.DepthStencil.Depth = depth;
            render_pass->render_pass_depth_stencil_desc_.StencilBeginningAccess.Clear.ClearValue.DepthStencil.Stencil = stencil;

            DescriptorAllocInfo alloc_info = frame_buffer->depth_stencil_->alloc_info_;
            D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { alloc_info.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + alloc_info.offset * device_->rtv_descriptor_offset_ };

            render_pass->render_pass_depth_stencil_desc_.cpuDescriptor = cpu_handle;
        }
        list_->BeginRenderPass(static_cast<uint32_t>(render_pass->render_pass_target_descs_.size()), render_pass->render_pass_target_descs_.data(), &render_pass->render_pass_depth_stencil_desc_, D3D12_RENDER_PASS_FLAG_NONE);
    } else {
        list_->BeginRenderPass(static_cast<uint32_t>(render_pass->render_pass_target_descs_.size()), render_pass->render_pass_target_descs_.data(), nullptr, D3D12_RENDER_PASS_FLAG_NONE);
    }
}

void CommandBuffer::EndRenderPass() {

    list_->EndRenderPass();
}

void CommandBuffer::TextureMemoryBarrier(Texture* texture, const MemoryState before, const MemoryState after) {

    D3D12_RESOURCE_BARRIER resource_barrier{};
    resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    resource_barrier.Transition.pResource = texture->resource_.Get();
    resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    resource_barrier.Transition.StateBefore = ToD3D12ResourceState(before);
    resource_barrier.Transition.StateAfter = ToD3D12ResourceState(after);

    list_->ResourceBarrier(1, &resource_barrier);
}

void CommandBuffer::BufferMemoryBarrier(Buffer* buffer, const MemoryState before, const MemoryState after) {

    D3D12_RESOURCE_BARRIER resource_barrier{};
    resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    resource_barrier.Transition.pResource = buffer->resource_.Get();
    resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    resource_barrier.Transition.StateBefore = ToD3D12ResourceState(before);
    resource_barrier.Transition.StateAfter = ToD3D12ResourceState(after);

    list_->ResourceBarrier(1, &resource_barrier);
}

void CommandBuffer::Reset() {

    THROW_IF_FAILED(allocator_->Reset());
    THROW_IF_FAILED(list_->Reset(allocator_.Get(), nullptr));
}

void CommandBuffer::Close() {

    THROW_IF_FAILED(list_->Close());
}

void CommandBuffer::DrawInstanced(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {

    list_->DrawInstanced(vertex_count, instance_count, first_vertex, first_instance);
}

void CommandBuffer::DrawIndexed(const uint32_t index_count, const uint32_t instance_count, const uint32_t first_index, const uint32_t vertex_offset, const uint32_t first_instance) {
    
    list_->DrawIndexedInstanced(index_count, instance_count, first_index, vertex_offset, first_instance);
}

void CommandBuffer::SetIndexBuffer(BufferView* buffer_view) {
    
    list_->IASetIndexBuffer(&buffer_view->index_view_desc_);
}

void CommandBuffer::SetVertexBuffer(const uint32_t slot, BufferView* buffer_view) {
    
    list_->IASetVertexBuffers(slot, 1, &buffer_view->vertex_view_desc_);
}

void CommandBuffer::CopyBuffer(Buffer* dst_buffer, const uint64_t dst_offset, Buffer* src_buffer, const uint64_t src_offset, const size_t size) {

    list_->CopyBufferRegion(dst_buffer->resource_.Get(), dst_offset, src_buffer->resource_.Get(), src_offset, size);
}
