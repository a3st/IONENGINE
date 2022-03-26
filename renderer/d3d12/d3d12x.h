// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <lib/exception.h>

#define NOMINMAX
#define UNICODE
#include <d3d12.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

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

}
