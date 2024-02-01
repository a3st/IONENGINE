// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "command_buffer.hpp"
#include "core/ref_ptr.hpp"
#include "pipeline_cache.hpp"
#define NOMINMAX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine::renderer::rhi
{
    inline uint32_t constexpr DX12_COMMAND_ALLOCATOR_CHUNK_DIRECT_MAX = 8;
    inline uint32_t constexpr DX12_COMMAND_ALLOCATOR_CHUNK_COPY_MAX = 4;
    inline uint32_t constexpr DX12_COMMAND_ALLOCATOR_CHUNK_COMPUTE_MAX = 4;
    inline uint32_t constexpr DX12_COMMAND_ALLOCATOR_CHUNK_BUNDLE_MAX = 12;

    class CommandAllocator : public core::ref_counted_object
    {
      public:
        CommandAllocator(ID3D12Device4* device, PipelineCache* pipeline_cache,
                         DescriptorAllocator* descriptor_allocator);

        auto allocate(D3D12_COMMAND_LIST_TYPE const list_type) -> core::ref_ptr<DX12CommandBuffer>;

        auto reset() -> void;

      private:
        struct Chunk
        {
            std::vector<core::ref_ptr<DX12CommandBuffer>> buffers;
            uint32_t offset;
            uint32_t size;
        };

        ID3D12Device4* device;
        PipelineCache* pipeline_cache;
        DescriptorAllocator* descriptor_allocator;
        std::unordered_map<D3D12_COMMAND_LIST_TYPE, winrt::com_ptr<ID3D12CommandAllocator>> allocators;
        std::unordered_map<D3D12_COMMAND_LIST_TYPE, std::vector<Chunk>> chunks;

        auto create_chunk(D3D12_COMMAND_LIST_TYPE const list_type) -> void;
    };
} // namespace ionengine::renderer::rhi