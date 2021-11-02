// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "command_buffer.h"
#include "device.h"
#include "texture.h"
#include "texture_view.h"
#include "render_pass.h"
#include "frame_buffer.h"
#include "conversion.h"

using namespace lgfx;

CommandBuffer::CommandBuffer(Device* device, const CommandBufferType type) : device_(device) {

    D3D12_COMMAND_LIST_TYPE list_type = ToD3D12CommandListType(type);
    THROW_IF_FAILED(device->device_->CreateCommandAllocator(list_type, __uuidof(ID3D12CommandAllocator), reinterpret_cast<void**>(allocator_.GetAddressOf())));
    THROW_IF_FAILED(device->device_->CreateCommandList(0, list_type, allocator_.Get(), nullptr, __uuidof(ID3D12GraphicsCommandList4), reinterpret_cast<void**>(list_.GetAddressOf())));
    THROW_IF_FAILED(list_->Close());
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

void CommandBuffer::BeginRenderPass(RenderPass* render_pass, FrameBuffer* frame_buffer, const ClearValueDesc& desc) {

    const RenderPassDesc& render_pass_desc = render_pass->GetDesc();
    const FrameBufferDesc& frame_buffer_desc = frame_buffer->GetDesc();

    for(uint32_t i = 0; i < static_cast<uint32_t>(render_pass_desc.colors.size()); ++i) {

        std::memcpy(render_pass->colors_desc_[i].BeginningAccess.Clear.ClearValue.Color, &desc.colors[i], sizeof(ClearValueColor));

        DescriptorAllocInfo alloc_info = frame_buffer_desc.colors[i]->alloc_info_;
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { alloc_info.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + alloc_info.offset * device_->rtv_descriptor_offset_ };

        render_pass->colors_desc_[i].cpuDescriptor = cpu_handle;
    }

    if(frame_buffer_desc.depth_stencil) {
        {
            render_pass->depth_stencil_desc_.DepthBeginningAccess.Clear.ClearValue.DepthStencil.Depth = desc.depth;
            render_pass->depth_stencil_desc_.StencilBeginningAccess.Clear.ClearValue.DepthStencil.Stencil = desc.stencil;

            DescriptorAllocInfo alloc_info = frame_buffer_desc.depth_stencil->alloc_info_;
            D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = { alloc_info.heap->heap_->GetCPUDescriptorHandleForHeapStart().ptr + alloc_info.offset * device_->rtv_descriptor_offset_ };

            render_pass->depth_stencil_desc_.cpuDescriptor = cpu_handle;
        }
        list_->BeginRenderPass(static_cast<uint32_t>(render_pass->colors_desc_.size()), render_pass->colors_desc_.data(), &render_pass->depth_stencil_desc_, D3D12_RENDER_PASS_FLAG_NONE);
    } else {
        list_->BeginRenderPass(static_cast<uint32_t>(render_pass->colors_desc_.size()), render_pass->colors_desc_.data(), nullptr, D3D12_RENDER_PASS_FLAG_NONE);
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

void CommandBuffer::Reset() {

    THROW_IF_FAILED(allocator_->Reset());
    THROW_IF_FAILED(list_->Reset(allocator_.Get(), nullptr));
}

void CommandBuffer::Close() {

    THROW_IF_FAILED(list_->Close());
}