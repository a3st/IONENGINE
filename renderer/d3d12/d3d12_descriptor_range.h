// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/d3d12/d3d12x.h>

namespace ionengine::renderer::backend::d3d12 {

class DescriptorRangeAllocation : public DescriptorAllocation {

    friend class DescriptorRange;

public:

    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle(uint32_t const index = 0) const override;

    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle(uint32_t const index = 0) const override;

protected:

    virtual void ReleaseThis() override;

private:

    ID3D12DescriptorHeap* _heap{nullptr};
    uint32_t _offset{0};
    uint32_t _descriptor_size{0};

    DescriptorRangeAllocation();

    virtual ~DescriptorRangeAllocation() = default;

    DescriptorRangeAllocation(DescriptorRangeAllocation const&) = delete;

    DescriptorRangeAllocation& operator=(DescriptorRangeAllocation const&) = delete;
};

class DescriptorRange : public IUnknownImpl {

    friend HRESULT create_descriptor_range(
        ID3D12Device4* const device, 
        ID3D12DescriptorHeap* const descriptor_heap,
        uint32_t const offset,
        uint32_t const size,
        DescriptorRange** const descriptor_range
    );

public:

    static uint32_t const MAX_ALLOCATION_COUNT = 64;

    HRESULT allocate(D3D12_DESCRIPTOR_RANGE const& descriptor_range, DescriptorAllocation** allocation);

    void reset();

    ID3D12DescriptorHeap* heap() const;

private:

    struct ArenaBlockRange {
        uint32_t begin;
        uint32_t size;
        uint32_t offset;
    };

    ID3D12DescriptorHeap* _heap{nullptr};
    uint32_t _offset{0};
    uint32_t _size{0};
    uint32_t _descriptor_size{0};

    std::vector<ArenaBlockRange> _arena_block_ranges;
    std::atomic<uint32_t> _arena_block_index{0};

    std::unique_ptr<DescriptorRangeAllocation[]> _allocations;

    HRESULT initialize(
        ID3D12Device4* const device, 
        ID3D12DescriptorHeap* const descriptor_heap,
        uint32_t const offset,
        uint32_t const size
    );

    DescriptorRange();

    virtual ~DescriptorRange() = default;

    DescriptorRange(DescriptorRange const&) = delete;

    DescriptorRange& operator=(DescriptorRange const&) = delete;
};

HRESULT create_descriptor_range(
    ID3D12Device4* const device, 
    ID3D12DescriptorHeap* const descriptor_heap,
    uint32_t const offset,
    uint32_t const size,
    DescriptorRange** const descriptor_range
);

}