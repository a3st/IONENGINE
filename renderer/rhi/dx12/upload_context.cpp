// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "upload_context.hpp"
#include "command_buffer.hpp"
#include "device.hpp"
#include "utils.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

UploadContext::UploadContext(
    ID3D12Device1* device, 
    ID3D12CommandQueue* queue, 
    ID3D12Fence* fence, 
    uint64_t& fence_value, 
    CommandAllocator& allocator,
    size_t const size
) : 
    queue(queue),
    fence(fence),
    fence_value(&fence_value),
    allocator(&allocator),
    size(size)
{
    auto d3d12_heap_desc = D3D12_HEAP_DESC {};
    d3d12_heap_desc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    d3d12_heap_desc.SizeInBytes = size;
    d3d12_heap_desc.Properties.Type = D3D12_HEAP_TYPE_UPLOAD;

    THROW_IF_FAILED(device->CreateHeap(&d3d12_heap_desc, __uuidof(ID3D12Heap), heap.put_void()));

    auto d3d12_resource_desc = D3D12_RESOURCE_DESC {};
    d3d12_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    d3d12_resource_desc.Width = size;
    d3d12_resource_desc.Height = 1;
    d3d12_resource_desc.MipLevels = 1;
    d3d12_resource_desc.DepthOrArraySize = 1;
    d3d12_resource_desc.SampleDesc.Count = 1;
    d3d12_resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    d3d12_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    THROW_IF_FAILED(device->CreatePlacedResource(
        heap.get(), 
        0, 
        &d3d12_resource_desc, 
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        __uuidof(ID3D12Resource),
        resource.put_void()
    ));
}

auto UploadContext::upload(ID3D12Resource* dst, std::span<uint8_t const> const data) -> void {

    if(data.size() > size - offset) {
        
    }

    auto range = D3D12_RANGE {};
    uint8_t* mapped_bytes = nullptr;
    THROW_IF_FAILED(resource->Map(0, &range, reinterpret_cast<void**>(&mapped_bytes)));
    std::memcpy(mapped_bytes + offset, data.data(), data.size());
    offset += data.size();

    auto command_buffer = allocator->allocate(D3D12_COMMAND_LIST_TYPE_COPY);
    command_buffer->close();

    auto command_batches = std::array<ID3D12CommandList*, 1> { 
        reinterpret_cast<ID3D12CommandList*>(static_cast<DX12CommandBuffer*>(command_buffer.get())->get_command_list())
    };

    queue->ExecuteCommandLists(static_cast<uint32_t>(command_batches.size()), command_batches.data());

    *fence_value++;
    THROW_IF_FAILED(queue->Signal(fence, *fence_value));


}