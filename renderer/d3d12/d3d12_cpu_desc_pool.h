// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/d3d12/d3d12x.h>

namespace ionengine::renderer::backend::d3d12 {

class IUnknownImpl : public IUnknown {
public:

    virtual ~IUnknownImpl() = default;

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject);

    virtual ULONG STDMETHODCALLTYPE AddRef();

    virtual ULONG STDMETHODCALLTYPE Release();

protected:

    virtual void ReleaseThis() { delete this; }

private:

    std::atomic<UINT> m_RefCount = 1;
};

struct DescriptorPoolImpl;
struct DescriptorPoolImpl_Deleter { void operator()(DescriptorPoolImpl* ptr) const; };

struct DescriptorAllocation : public IUnknownImpl {
public:

    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle(uint32_t const index = 0) const;

    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle(uint32_t const index = 0) const;

private:

    friend class DescriptorPoolImpl;

    ID3D12DescriptorHeap* _heap{nullptr};
    uint32_t _offset{0};
    uint32_t _descriptor_size{0};

    DescriptorAllocation(DescriptorPoolImpl* const pool, uint32_t const offset, uint32_t const descriptor_size);

    ~DescriptorAllocation();

    DescriptorAllocation(DescriptorAllocation const&) = delete;

    DescriptorAllocation& operator=(DescriptorAllocation const&) = delete;
};

class CPUDescriptorPool : public IUnknownImpl {
public:

    static uint32_t const DESCRIPTOR_HEAP_SIZE = 256;

    HRESULT allocate(DescriptorAllocation** allocation);
    
    HRESULT deallocate(DescriptorAllocation* allocation);

private:

    std::unique_ptr<DescriptorPoolImpl, DescriptorPoolImpl_Deleter> _impl;

    CPUDescriptorPool(ID3D12Device4* const device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size);

    CPUDescriptorPool(CPUDescriptorPool const&) = delete;

    CPUDescriptorPool& operator=(CPUDescriptorPool const&) = delete;
};

}
