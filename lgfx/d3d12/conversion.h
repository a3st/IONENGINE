// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

extern D3D12_RESOURCE_DIMENSION ToD3D12ResourceDimension(const Dimension dimension);
extern D3D12_HEAP_TYPE ToD3D12HeapType(const MemoryType type);
extern D3D12_DESCRIPTOR_HEAP_TYPE ToD3D12DescriptorHeapType(const DescriptorType type);
extern D3D12_HEAP_FLAGS ToD3D12HeapFlags(const MemoryFlags flags);
extern D3D12_COMPARISON_FUNC ToD3D12ComparisonFunc(const ComparisonFunc func);
extern D3D12_FILTER ToD3D12Filter(const Filter filter);
extern D3D12_TEXTURE_ADDRESS_MODE ToD3D12TextureAddressMode(const TextureAddressMode mode);
extern Format DXGIFormatTo(const DXGI_FORMAT format);
extern DXGI_FORMAT ToDXGIFormat(const Format format);
extern D3D12_COMMAND_LIST_TYPE ToD3D12CommandListType(const CommandBufferType type);
extern D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE ToD3D12RenderPassBeginType(const RenderPassLoadOp op);
extern D3D12_RENDER_PASS_ENDING_ACCESS_TYPE ToD3D12RenderPassEndType(const RenderPassStoreOp op);
extern D3D12_RESOURCE_STATES ToD3D12ResourceState(const MemoryState state);

}