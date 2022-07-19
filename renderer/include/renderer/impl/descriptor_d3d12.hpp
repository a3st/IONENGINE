// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#define NOMINMAX
#include <d3d12.h>
#include <wrl/client.h>

using namespace Microsoft;

namespace ionengine::renderer {

class DescriptorAllocation;

enum class DescriptorPoolFlags { None = 0, Free = 1 << 0 };

DECLARE_ENUM_CLASS_BIT_FLAG(DescriptorPoolFlags)

class DescriptorPool : public IUnknown {
 public:
    virtual HRESULT QueryInterface(REFIID riid, void** ppvObject);

    virtual ULONG AddRef();

    virtual ULONG Release();

    virtual void ReleaseThis();

    virtual HRESULT Allocate(uint32_t const size,
                             DescriptorAllocation** ppAllocation) = 0;

    virtual void Free(DescriptorAllocation* pAllocation) = 0;

    virtual void SetCurrentFrameIndex(uint32_t const frameIndex) = 0;

    virtual void Reset() = 0;

    virtual ID3D12DescriptorHeap* GetHeap() = 0;

 private:
    std::atomic<UINT> _ref_count = 1;
};

///
/// Descriptor Pool should know thread count used by application
/// This variable should be changed until descriptor pool not used
///
inline uint16_t DESCRIPTOR_POOL_THREAD_COUNT = 12;

class DescriptorAllocation : public IUnknown {
 public:
    virtual HRESULT QueryInterface(REFIID riid, void** ppvObject);

    virtual ULONG AddRef();

    virtual ULONG Release();

    virtual void ReleaseThis();

    inline D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle(uint32_t const index = 0) const {
        return D3D12_CPU_DESCRIPTOR_HANDLE{
            .ptr = _pPool->GetHeap()->GetCPUDescriptorHandleForHeapStart().ptr +
                   _descriptorSize * _descriptorIndex + index};
    }

    inline D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle(uint32_t const index = 0) const {
        return D3D12_GPU_DESCRIPTOR_HANDLE{
            .ptr = _pPool->GetHeap()->GetGPUDescriptorHandleForHeapStart().ptr +
                   _descriptorSize * _descriptorIndex + index};
    }

    inline uint32_t NumDescriptors() const { return _descriptorCount; }

 private:
    void Initialize(DescriptorPool* pPool, uint32_t const descriptorIndex,
                    uint32_t const descriptorSize,
                    uint32_t const descriptorCount);
    void Deinitialize();

    uint32_t _descriptorIndex;
    uint32_t _descriptorSize;
    uint32_t _descriptorCount;
    DescriptorPool* _pPool;

    std::atomic<UINT> _ref_count = 1;

    friend class DescriptorLinearPool;
    friend class DescriptorBlockPool;
};

class DescriptorLinearPool : public DescriptorPool {
 public:
 private:
    struct ThreadPoolData {
        uint32_t begin;
        uint32_t offset;
        std::vector<uint32_t> free;
    };

    std::unordered_map<std::thread::id, ThreadPoolData> _thread_data;
    WRL::ComPtr<ID3D12DescriptorHeap> _heap;
    uint32_t _frameIndex;

    friend HRESULT CreateDescriptorPool(
        ID3D12Device4* const pDevice, D3D12_DESCRIPTOR_HEAP_TYPE const heapType,
        uint32_t const heapSize, bool const shaderVisible,
        DescriptorPoolFlags const poolFlags,
        DescriptorPool** const ppDescriptorPool);
};

class DescriptorBlockPool : public DescriptorPool {
 public:
    virtual HRESULT Allocate(uint32_t const size,
                             DescriptorAllocation** ppAllocation) override;

    virtual void SetCurrentFrameIndex(uint32_t const frameIndex) override;

    virtual void Free(DescriptorAllocation* pAllocation) override;

    virtual void Reset() override;

    inline virtual ID3D12DescriptorHeap* GetHeap() override { return _heap.Get(); }

 private:
    WRL::ComPtr<ID3D12DescriptorHeap> _heap;
    std::mutex _mutex;
    std::vector<uint32_t> _free;
    std::vector<DescriptorAllocation*> _allocations;
    uint32_t _descriptorSize;

    friend HRESULT CreateDescriptorPool(
        ID3D12Device4* const pDevice, D3D12_DESCRIPTOR_HEAP_TYPE const heapType,
        uint32_t const heapSize, bool const shaderVisible,
        DescriptorPoolFlags const poolFlags,
        DescriptorPool** const ppDescriptorPool);
};

HRESULT CreateDescriptorPool(ID3D12Device4* const pDevice,
                             D3D12_DESCRIPTOR_HEAP_TYPE const heapType,
                             uint32_t const heapSize, bool const shaderVisible,
                             DescriptorPoolFlags const poolFlags,
                             DescriptorPool** const ppDescriptorPool);

}  // namespace ionengine::renderer