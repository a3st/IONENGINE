// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/exception.h>

#define NOMINMAX
#define UNICODE
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>
#include <dxcapi.h>

#ifndef THROW_IF_FAILED
#define THROW_IF_FAILED(Result) if(FAILED(Result)) throw ionengine::lib::Exception(ionengine::renderer::backend::d3d12::hresult_to_string(Result));
#endif // THROW_IF_FAILED

using Microsoft::WRL::ComPtr;

namespace ionengine::renderer::backend::d3d12 {

inline std::u8string hresult_to_string(HRESULT const result) {

	switch(result) {
		case E_FAIL: return u8"Attempted to create a device with the debug layer enabled and the layer is not installed";
		case E_INVALIDARG: return u8"An invalid parameter was passed to the returning function";
		case E_OUTOFMEMORY: return u8"Direct3D could not allocate sufficient memory to complete the call";
		case E_NOTIMPL: return u8"The method call isn't implemented with the passed parameter combination";
		case S_FALSE: return u8"Alternate success value, indicating a successful but nonstandard completion";
		case S_OK: return u8"No error occurred";
		case D3D12_ERROR_ADAPTER_NOT_FOUND: return u8"The specified cached PSO was created on a different adapter and cannot be reused on the current adapter";
		case D3D12_ERROR_DRIVER_VERSION_MISMATCH: return u8"The specified cached PSO was created on a different driver version and cannot be reused on the current adapter";
		case DXGI_ERROR_INVALID_CALL: return u8"The method call is invalid. For example, a method's parameter may not be a valid pointer";
		case DXGI_ERROR_WAS_STILL_DRAWING: return u8"The previous blit operation that is transferring information to or from this surface is incomplete";
		default: return u8"An unknown error has occurred";
	}
}

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

class DescriptorAllocation : public IUnknownImpl {
public:

	virtual ~DescriptorAllocation() = default;

	virtual D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle(uint32_t const index = 0) const = 0;

    virtual D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle(uint32_t const index = 0) const = 0;

protected:

	virtual void ReleaseThis() { delete this; }
};

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

protected:

    virtual void ReleaseThis() override;

private:

    std::vector<detail::DescriptorHeap> _heaps;
    DescriptorPoolAllocation* _allocations;
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

    static uint32_t const MAX_ALLOCATION_COUNT = 256;

    HRESULT allocate(D3D12_DESCRIPTOR_RANGE const& descriptor_range, DescriptorAllocation** allocation);

    void reset();

    ID3D12DescriptorHeap* heap() const;

protected:

    virtual void ReleaseThis() override;

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

    DescriptorRangeAllocation* _allocations;

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
