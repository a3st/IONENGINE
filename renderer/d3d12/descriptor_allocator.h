// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/d3d12/d3d12_shared.h>

namespace ionengine::renderer::d3d12 {

struct DescriptorHeap {
    uint32_t heap_size;
    ComPtr<ID3D12DescriptorHeap> heap;
    std::vector<uint8_t> blocks;
    uint32_t offset;
};

struct DescriptorAllocInfo {
    DescriptorHeap* _heap;
    uint32_t _offset;

    ID3D12DescriptorHeap* heap() const { return _heap->heap.Get(); }
    uint32_t offset() const { return _offset; }
};

class DescriptorAllocator {
public:

    DescriptorAllocator() = default;

    DescriptorAllocator(DescriptorAllocator const&) = delete;

    DescriptorAllocator(DescriptorAllocator&&) noexcept = default;

    DescriptorAllocator& operator=(DescriptorAllocator const&) = delete;

    DescriptorAllocator& operator=(DescriptorAllocator&&) = default;

    DescriptorAllocInfo allocate(ID3D12Device4* device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, bool const shader_visible);
    
    void deallocate(DescriptorAllocInfo const& alloc_info);

    void reset();

private:

    using DescriptorPool = std::pair<std::list<DescriptorHeap>, uint32_t>;

    enum class HeapTypeIndex : uint16_t {
        SRV_CBV_UAV = 1,
        RTV = 2,
        DSV = 3,
        Sampler = 4,
        SRV_CBV_UAV_Shader = 5,
        Sampler_Shader = 6
    };

    std::array<DescriptorPool, 6> _pools;

    enum class HeapSize : uint32_t {
        _4096 = 4096,
        _2048 = 2048,
        _1024 = 1024,
        _256 = 256,
        _128 = 128
    };

    void create_descriptor_pool(ID3D12Device4* const device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, bool const shader_visible);

    DescriptorAllocInfo get_free_block(ID3D12Device4* const device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, bool const shader_visible);
};

}
