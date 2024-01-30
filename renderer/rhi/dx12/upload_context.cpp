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

    if(src->get_flags() & BufferUsage::MapWrite) {
        uint8_t* mapped_bytes;
        THROW_IF_FAILED(static_cast<DX12Buffer*>(src.get())->get_resource()->Map(0, nullptr, reinterpret_cast<void**>(&mapped_bytes)));
        std::memcpy(mapped_bytes, data.data(), data.size());
        static_cast<DX12Buffer*>(src.get())->get_resource()->Unmap(0, nullptr);
    } else {
        if(data.size() > buffer->get_size() - offset) {
            if(fence->GetCompletedValue() < *fence_value) {
                THROW_IF_FAILED(fence->SetEventOnCompletion(*fence_value, fence_event));
                ::WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
            }

            command_allocator->reset();
            offset = 0;
        }

        uint8_t* mapped_bytes;
        THROW_IF_FAILED(buffer->get_resource()->Map(0, nullptr, reinterpret_cast<void**>(&mapped_bytes)));
        std::memcpy(mapped_bytes + offset, data.data(), data.size());
        buffer->get_resource()->Unmap(0, nullptr);

        auto command_buffer = command_allocator->allocate(D3D12_COMMAND_LIST_TYPE_COPY);
        command_buffer->reset();
        command_buffer->copy_buffer(src, 0, buffer, offset, data.size());
        command_buffer->close();

        offset += data.size();

        auto command_batches = std::array<ID3D12CommandList*, 1> { 
            reinterpret_cast<ID3D12CommandList*>(static_cast<DX12CommandBuffer*>(command_buffer.get())->get_command_list())
        };

        queue->ExecuteCommandLists(static_cast<uint32_t>(command_batches.size()), command_batches.data());
        (*fence_value)++;
        THROW_IF_FAILED(queue->Signal(fence, *fence_value));
    }
}

auto UploadContext::upload(core::ref_ptr<Texture> src, std::vector<std::span<uint8_t const>> const& data) -> void {

    /*if(data.size() > buffer->get_size() - offset) {
        if(fence->GetCompletedValue() < *fence_value) {
            THROW_IF_FAILED(fence->SetEventOnCompletion(*fence_value, fence_event));
            ::WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
        }

        command_allocator->reset();
        offset = 0;
    }*/

    auto target = static_cast<DX12Texture*>(src.get());

    uint32_t mip_width = target->get_width();
    uint32_t mip_height = target->get_height();

    size_t row_bytes = 0;
    uint32_t row_count = 0;

    uint32_t const row_pitch_alignment_mask = 512 - 1;
    uint32_t const resource_alignment_mask = 256 - 1;
    uint32_t const stride = 4;

    uint8_t* mapped_bytes;
    THROW_IF_FAILED(buffer->get_resource()->Map(0, nullptr, reinterpret_cast<void**>(&mapped_bytes)));

    std::vector<rhi::TextureCopyRegion> regions;
    for(size_t i = 0; i < data.size(); ++i) {
        if(data[i].empty()) {
            break;
        }

        get_texture_data(target->get_format(), mip_width, mip_height, row_bytes, row_count);

        uint32_t const row_pitch = (mip_width * stride + row_pitch_alignment_mask) & ~row_pitch_alignment_mask;

        auto region = rhi::TextureCopyRegion { 
            .mip_width = mip_width,
            .mip_height = mip_height,
            .mip_depth = target->get_depth(),
            .row_pitch = row_pitch,
            .offset = offset
        };
        regions.emplace_back(region);

        for(uint32_t j = 0; j < row_count; ++j) {
            std::memcpy(mapped_bytes + offset + row_pitch * j, data[i].data() + row_bytes * j, row_bytes);
        }

        mip_width = std::max<uint32_t>(1, mip_width / 2);
        mip_height = std::max<uint32_t>(1, mip_height / 2);

        offset += (row_count * row_pitch + resource_alignment_mask) & ~resource_alignment_mask;
    }

    buffer->get_resource()->Unmap(0, nullptr);

    auto command_buffer = command_allocator->allocate(D3D12_COMMAND_LIST_TYPE_COPY);
    command_buffer->reset();
    command_buffer->copy_buffer(src, buffer, regions);
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

auto UploadContext::get_texture_data(
    rhi::TextureFormat const format, 
    uint32_t const mip_width, 
    uint32_t const mip_height, 
    size_t& row_bytes, 
    uint32_t& row_count
) -> void 
{
    bool bc = false;
    uint32_t bpe = 0;

    switch(format) {
        case rhi::TextureFormat::BC1:
        case rhi::TextureFormat::BC4: {
            bc = true;
            bpe = 8;
        } break;
        case rhi::TextureFormat::BC3:
        case rhi::TextureFormat::BC5: {
            bc = true;
            bpe = 16;
        } break;
    }

    if(bc) {
        uint64_t const block_wide = std::max<uint64_t>(1u, (static_cast<uint64_t>(mip_width) + 3u) / 4u);
        uint64_t const block_high = std::max<uint64_t>(1u, (static_cast<uint64_t>(mip_height) + 3u) / 4u);
        row_bytes = block_wide * bpe;
        row_count = static_cast<uint32_t>(block_high);
    } else {
        size_t const bpp = 32;
        row_bytes = (mip_width * bpp + 7) / 8;
        row_count = mip_height;
    }
}