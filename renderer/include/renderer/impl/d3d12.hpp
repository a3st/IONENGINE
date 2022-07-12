// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#define NOMINMAX
#include <d3d12.h>
#include <d3d12ma/D3D12MemAlloc.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

using namespace Microsoft;

namespace ionengine::renderer {

inline std::string to_string(HRESULT const result) {
    switch (result) {
        case E_FAIL:
            return "E_FAIL";
        case E_INVALIDARG:
            return "E_INVALIDARG";
        case E_OUTOFMEMORY:
            return "E_OUTOFMEMORY";
        case E_NOTIMPL:
            return "E_NOTIMPL";
        case S_FALSE:
            return "S_FALSE";
        case DXGI_ERROR_DEVICE_HUNG:
            return "DXGI_ERROR_DEVICE_HUNG";
        case DXGI_ERROR_DEVICE_REMOVED:
            return "DXGI_ERROR_DEVICE_REMOVED";
        case D3D12_ERROR_ADAPTER_NOT_FOUND:
            return "D3D12_ERROR_ADAPTER_NOT_FOUND";
        case D3D12_ERROR_DRIVER_VERSION_MISMATCH:
            return "D3D12_ERROR_DRIVER_VERSION_MISMATCH";
        case DXGI_ERROR_INVALID_CALL:
            return "DXGI_ERROR_INVALID_CALL";
        case DXGI_ERROR_WAS_STILL_DRAWING:
            return "DXGI_ERROR_WAS_STILL_DRAWING";
        case DXGI_ERROR_DEVICE_RESET:
            return "DXGI_ERROR_DEVICE_RESET";
        default:
            return "DXGI_ERROR_UNKNOWN";
    }
}

}  // namespace ionengine::renderer