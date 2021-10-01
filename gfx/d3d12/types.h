// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DFence;
class D3DCommandList;
class D3DResource;
class D3DSampler;
class D3DView;
class D3DRenderPass;
class D3DFrameBuffer;
class D3DDescriptorSetLayout;
class D3DDescriptorSet;
class D3DPipeline;
class D3DDevice;

struct D3DDescriptorTable {
	D3D12_DESCRIPTOR_HEAP_TYPE type;
	uint32 count;
	uint32 offset;
	bool compute;
};

struct D3DMemoryHeap {
    winrt::com_ptr<ID3D12Heap> d3d12_heap;
    usize heap_size;
    uint64 block_count;
    usize block_size;
    uint64 offset;
    std::vector<uint8> block_data;
};

struct D3DMemoryPtr {
    D3DMemoryHeap* heap;
    uint64 offset;
};

struct D3DDescriptorHeap {
	winrt::com_ptr<ID3D12DescriptorHeap> d3d12_heap;
	uint32 heap_size;
	uint32 offset;
	std::vector<uint8> descriptor_data;
};

struct D3DDescriptorPtr {
	D3DDescriptorHeap* heap;
	uint32 offset;
};

std::string result_to_string(const HRESULT result) {
	switch(result) {
		case E_FAIL: return "Attempted to create a device with the debug layer enabled and the layer is not installed";
		case E_INVALIDARG: return "An invalid parameter was passed to the returning function";
		case E_OUTOFMEMORY: return "Direct3D could not allocate sufficient memory to complete the call";
		case E_NOTIMPL: return "The method call isn't implemented with the passed parameter combination";
		case S_FALSE: return "Alternate success value, indicating a successful but nonstandard completion";
		case S_OK: return "No error occurred";
		case D3D12_ERROR_ADAPTER_NOT_FOUND: return "The specified cached PSO was created on a different adapter and cannot be reused on the current adapter";
		case D3D12_ERROR_DRIVER_VERSION_MISMATCH: return "The specified cached PSO was created on a different driver version and cannot be reused on the current adapter";
		case DXGI_ERROR_INVALID_CALL: return "The method call is invalid. For example, a method's parameter may not be a valid pointer";
		case DXGI_ERROR_WAS_STILL_DRAWING: return "The previous blit operation that is transferring information to or from this surface is incomplete";
		default: return "An unknown error has occurred";
	}
}

extern constexpr char* api_name = "D3D12";

}

#ifndef THROW_IF_FAILED
#define THROW_IF_FAILED(HResult) if(FAILED(HResult)) throw std::runtime_error(result_to_string(HResult));
#endif
