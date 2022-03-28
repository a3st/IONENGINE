// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/d3d12/d3d12x.h>

using namespace ionengine::renderer::backend::d3d12;

HRESULT STDMETHODCALLTYPE IUnknownImpl::QueryInterface(REFIID riid, void** ppvObject) {

    if (ppvObject == NULL) {
        return E_POINTER;
    }

    if (riid == IID_IUnknown) {
        ++m_RefCount;
        *ppvObject = this;
        return S_OK;
    }

    *ppvObject = NULL;
    return E_NOINTERFACE;
}

ULONG STDMETHODCALLTYPE IUnknownImpl::AddRef() {

    return ++m_RefCount;
}

ULONG STDMETHODCALLTYPE IUnknownImpl::Release() {

    const uint32_t newRefCount = --m_RefCount;

    if (newRefCount == 0) {
        ReleaseThis();
    }

    return newRefCount;
}
