// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Instance final {
friend class Adapter;
friend class Device;
friend class Swapchain;
public:

    Instance() {
        uint32 factory_flags = 0;
        
#ifdef NDEBUG
        factory_flags = DXGI_CREATE_FACTORY_DEBUG;
#endif

        throw_if_failed(CreateDXGIFactory2(factory_flags, IID_PPV_ARGS(&m_factory_ptr)));

#ifdef NDEBUG
        throw_if_failed(D3D12GetDebugInterface(IID_PPV_ARGS(&m_debug_ptr)));
        m_debug_ptr->EnableDebugLayer();
#endif
    }

private:

    ComPtr<IDXGIFactory4> m_factory_ptr;
    ComPtr<ID3D12Debug> m_debug_ptr;
};

}