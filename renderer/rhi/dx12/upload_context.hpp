// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "buffer.hpp"
#include "command_allocator.hpp"
#include "core/ref_ptr.hpp"
#include "memory_allocator.hpp"
#define NOMINMAX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine::renderer::rhi
{
    inline uint32_t constexpr DX12_RESOURCE_MEMORY_ALIGNMENT = 256;
    inline uint32_t constexpr DX12_TEXTURE_ROW_PITH_ALIGNMENT = 512;

    class UploadContext : public core::ref_counted_object
    {
      public:
        UploadContext(ID3D12Device4* device, ID3D12CommandQueue* queue, ID3D12Fence* fence, HANDLE fence_event,
                      uint64_t& fence_value, size_t const size);

        auto upload(core::ref_ptr<Buffer> src, std::span<uint8_t const> const data) -> void;

        auto upload(core::ref_ptr<Texture> src, std::vector<std::span<uint8_t const>> const& data) -> void;

        auto try_reset() -> void;

      private:
        ID3D12CommandQueue* queue;
        ID3D12Fence* fence;
        HANDLE fence_event;
        uint64_t* fence_value;
        core::ref_ptr<DX12MemoryAllocator> memory_allocator;
        core::ref_ptr<CommandAllocator> command_allocator;
        core::ref_ptr<DX12Buffer> buffer;
        uint64_t offset;

        auto get_texture_data(rhi::TextureFormat const format, uint32_t const mip_width, uint32_t const mip_height,
                              size_t& row_bytes, uint32_t& row_count) -> void;
    };
} // namespace ionengine::renderer::rhi