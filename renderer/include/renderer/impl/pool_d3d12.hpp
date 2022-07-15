// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#define NOMINMAX
#include <d3d12.h>
#include <wrl/client.h>

using namespace Microsoft;

namespace ionengine::renderer {

///
/// Descriptor Pool should know thread count used by application
/// This variable should be changed until descriptor pool not used
///
inline uint16_t DESCRIPTOR_POOL_THREAD_COUNT = 12;

class DescriptorPool : public IUnknown {
 public:
    virtual HRESULT QueryInterface(REFIID riid, void** ppvObject);

    virtual ULONG AddRef();

    virtual ULONG Release();

    virtual void ReleaseThis();

    HRESULT allocate(uint32_t const size);

 private:
    struct ThreadPoolData {
        uint32_t begin;
        uint32_t offset;
        std::vector<uint32_t> descriptors;
    };

    std::unordered_map<std::thread::id, ThreadPoolData> _thread_data;
    WRL::ComPtr<ID3D12DescriptorHeap> _heap;
    std::shared_mutex _mutex;
    bool _free_descriptors;

    std::atomic<UINT> _ref_count = 1;

    friend HRESULT create_descriptor_pool(
        ID3D12Device4* const device, D3D12_DESCRIPTOR_HEAP_TYPE const heap_type,
        uint32_t const heap_size, bool const shader_visible,
        DescriptorPool** const descriptor_pool);
};

HRESULT create_descriptor_pool(ID3D12Device4* const device,
                               D3D12_DESCRIPTOR_HEAP_TYPE const heap_type,
                               uint32_t const heap_size,
                               bool const shader_visible,
                               DescriptorPool** const descriptor_pool);

}  // namespace ionengine::renderer