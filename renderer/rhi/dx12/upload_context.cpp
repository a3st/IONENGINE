// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "upload_context.hpp"
#include "command_buffer.hpp"
#include "buffer.hpp"
#include "utils.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

UploadContext::UploadContext(
    ID3D12Device4* device, 
    ID3D12CommandQueue* queue, 
    ID3D12Fence* fence, 
    HANDLE fence_event,
    uint64_t& fence_value,
    size_t const size
) : 
    queue(queue),
    fence(fence),
    fence_event(fence_event),
    fence_value(&fence_value)
{
    command_allocator = core::make_ref<CommandAllocator>(device, nullptr, nullptr);
    memory_allocator = core::make_ref<DX12MemoryAllocator>(
        device, 
        1 * 1024 * 1024, 
        32 * 1024 * 1024, 
        (BufferUsageFlags)BufferUsage::MapWrite
    );
    buffer = core::make_ref<DX12Buffer>(
        device,
        &memory_allocator, 
        nullptr, 
        32 * 1024 * 1024, 
        (BufferUsageFlags)(BufferUsage::MapWrite | BufferUsage::CopySrc)
    );
}

auto UploadContext::upload(core::ref_ptr<Buffer> src, std::span<uint8_t const> const data) -> void {

    if(data.size() > buffer->get_size() - offset) {
        
        if(fence->GetCompletedValue() < *fence_value) {
            THROW_IF_FAILED(fence->SetEventOnCompletion(*fence_value, fence_event));
            ::WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
        }

        command_allocator->reset();
        offset = 0;
    }

    auto range = D3D12_RANGE {};
    uint8_t* mapped_bytes = nullptr;
    THROW_IF_FAILED(buffer->get_resource()->Map(0, &range, reinterpret_cast<void**>(&mapped_bytes)));
    std::memcpy(mapped_bytes + offset, data.data(), data.size());
    offset += data.size();
    buffer->get_resource()->Unmap(0, &range);

    auto command_buffer = command_allocator->allocate(D3D12_COMMAND_LIST_TYPE_COPY);
    command_buffer->reset();
    command_buffer->copy_buffer(src, 0, buffer, 0, data.size());
    command_buffer->close();

    auto command_batches = std::array<ID3D12CommandList*, 1> { 
        reinterpret_cast<ID3D12CommandList*>(static_cast<DX12CommandBuffer*>(command_buffer.get())->get_command_list())
    };

    queue->ExecuteCommandLists(static_cast<uint32_t>(command_batches.size()), command_batches.data());
    (*fence_value)++;
    THROW_IF_FAILED(queue->Signal(fence, *fence_value));
}

auto UploadContext::try_reset() -> void {

    if(fence->GetCompletedValue() >= *fence_value) {
        command_allocator->reset();
        offset = 0;
    }
}
