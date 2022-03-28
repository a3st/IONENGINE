// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/d3d12/d3d12x.h>

namespace ionengine::renderer::backend::d3d12 {

namespace detail {

struct DescriptorHeap {
    ComPtr<ID3D12DescriptorHeap> heap;
    std::list<uint32_t> indices;
    uint32_t offset;
};

}

class DescriptorPoolAllocation : public DescriptorAllocation {

    friend class DescriptorPool;

public:

    virtual D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle(uint32_t const index = 0) const override;

    virtual D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle(uint32_t const index = 0) const override;

protected:

    virtual void ReleaseThis() override;

private:

    detail::DescriptorHeap* _heap{nullptr};
    uint32_t _offset{0};
    uint32_t _descriptor_size{0};

    DescriptorPoolAllocation();

    virtual ~DescriptorPoolAllocation() = default;

    DescriptorPoolAllocation(DescriptorPoolAllocation const&) = delete;

    DescriptorPoolAllocation& operator=(DescriptorPoolAllocation const&) = delete;
};

class DescriptorPool : public IUnknownImpl {

    friend HRESULT create_descriptor_pool(
        ID3D12Device4* const device, 
        D3D12_DESCRIPTOR_HEAP_TYPE const heap_type,
        D3D12_DESCRIPTOR_HEAP_FLAGS const heap_flags,
        uint32_t const size, 
        DescriptorPool** const descriptor_pool
    );

public:

    static uint32_t const DESCRIPTOR_HEAP_SIZE = 256;

    HRESULT allocate(DescriptorAllocation** allocation);

private:

    std::vector<detail::DescriptorHeap> _heaps;
    std::unique_ptr<DescriptorPoolAllocation[]> _allocations;
    uint32_t _descriptor_size{0};

    HRESULT initialize(
        ID3D12Device4* const device, 
        D3D12_DESCRIPTOR_HEAP_TYPE const heap_type,
        D3D12_DESCRIPTOR_HEAP_FLAGS const heap_flags,
        uint32_t const size
    );
        
    DescriptorPool();

    virtual ~DescriptorPool() = default;

    DescriptorPool(DescriptorPool const&) = delete;

    DescriptorPool& operator=(DescriptorPool const&) = delete;
};

HRESULT create_descriptor_pool(
    ID3D12Device4* const device, 
    D3D12_DESCRIPTOR_HEAP_TYPE const heap_type,
    D3D12_DESCRIPTOR_HEAP_FLAGS const heap_flags, 
    uint32_t const size,
    DescriptorPool** const descriptor_pool
);

}
