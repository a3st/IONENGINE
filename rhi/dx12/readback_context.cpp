// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "readback_context.hpp"
#include "precompiled.h"
#include "utils.hpp"

namespace ionengine::renderer::rhi
{
    ReadbackContext::ReadbackContext(ID3D12Device4* device, ID3D12CommandQueue* queue, ID3D12Fence* fence,
                                     HANDLE fence_event, uint64_t& fence_value, size_t const size)
        : device(device), queue(queue), fence(fence), fence_event(fence_event), fence_value(&fence_value),
          command_allocator(core::make_ref<CommandAllocator>(device, nullptr, nullptr)),
          memory_allocator(
              core::make_ref<DX12MemoryAllocator>(device, 1 * 1024 * 1024, size, MemoryAllocatorUsage::Readback)),
          buffer(core::make_ref<DX12Buffer>(device, *memory_allocator, nullptr, size,
                                            (BufferUsageFlags)(BufferUsage::MapRead | BufferUsage::CopyDst)))
    {
    }

    auto ReadbackContext::read(core::ref_ptr<Texture> src, std::vector<std::vector<uint8_t>>& data) -> void
    {
        auto d3d12_resource_desc = static_cast<DX12Texture*>(src.get())->get_resource()->GetDesc();
        std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> d3d12_subresource_footprints;
        size_t total_size;
        d3d12_subresource_footprints.resize(d3d12_resource_desc.MipLevels);
        device->GetCopyableFootprints(&d3d12_resource_desc, 0, d3d12_resource_desc.MipLevels, 0, nullptr, nullptr,
                                      nullptr, &total_size);

        std::vector<TextureCopyRegion> regions;
        for (auto const& footprint : d3d12_subresource_footprints)
        {
            auto region = rhi::TextureCopyRegion{.mip_width = footprint.Footprint.Width,
                                                 .mip_height = footprint.Footprint.Height,
                                                 .mip_depth = footprint.Footprint.Depth,
                                                 .row_pitch = footprint.Footprint.RowPitch,
                                                 .offset = footprint.Offset};
            regions.emplace_back(region);
        }

        auto command_buffer = command_allocator->allocate(D3D12_COMMAND_LIST_TYPE_COPY);
        command_buffer->reset();
        command_buffer->copy_texture(buffer, src, regions);
        command_buffer->close();

        auto command_batches = std::array<ID3D12CommandList*, 1>{reinterpret_cast<ID3D12CommandList*>(
            static_cast<DX12CommandBuffer*>(command_buffer.get())->get_command_list())};

        queue->ExecuteCommandLists(static_cast<uint32_t>(command_batches.size()), command_batches.data());
        (*fence_value)++;
        THROW_IF_FAILED(queue->Signal(fence, *fence_value));

        if (fence->GetCompletedValue() < *fence_value)
        {
            THROW_IF_FAILED(fence->SetEventOnCompletion(*fence_value, fence_event));
            ::WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
        }

        command_allocator->reset();

        uint8_t* mapped_bytes = nullptr;
        auto range = D3D12_RANGE{.Begin = 0, .End = total_size};
        THROW_IF_FAILED(buffer->get_resource()->Map(0, &range, reinterpret_cast<void**>(&mapped_bytes)));

        data.resize(d3d12_resource_desc.MipLevels);

        size_t row_bytes;
        uint32_t row_count;

        for (uint32_t const i : std::views::iota(0u, data.size()))
        {
            get_texture_data(src->get_format(), d3d12_subresource_footprints[i].Footprint.Width,
                             d3d12_subresource_footprints[i].Footprint.Height, row_bytes, row_count);
        }

        buffer->get_resource()->Unmap(0, nullptr);
    }
} // namespace ionengine::renderer::rhi