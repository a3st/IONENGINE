// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "command_buffer.hpp"
#include "pipeline_cache.hpp"
#define NOMINMAX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine {

namespace renderer {

namespace rhi {

class CommandAllocator : public core::ref_counted_object {
public:

    CommandAllocator(
        ID3D12Device4* device,
        PipelineCache* pipeline_cache, 
        DescriptorAllocator* descriptor_allocator
    );

    auto allocate(D3D12_COMMAND_LIST_TYPE const list_type) -> core::ref_ptr<DX12CommandBuffer>;

    auto reset() -> void;

private:

    inline static uint32_t CHUNK_COMMAND_LIST_DIRECT_MAX = 8;
    inline static uint32_t CHUNK_COMMAND_LIST_COPY_MAX = 4;
    inline static uint32_t CHUNK_COMMAND_LIST_COMPUTE_MAX = 4;

    struct Chunk {
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

}

}

}