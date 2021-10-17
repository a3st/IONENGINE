// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

extern D3D12_HEAP_TYPE GFXToD3D12HeapType(const MemoryType type);
extern D3D12_DESCRIPTOR_HEAP_TYPE GFXToD3D12DescriptorHeapType(const DescriptorType type);
extern D3D12_HEAP_FLAGS GFXToD3D12HeapFlags(const MemoryFlags flags);
extern D3D12_COMPARISON_FUNC GFXToD3D12ComparisonFunc(const ComparisonFunc func);
extern D3D12_FILTER GFXToD3D12Filter(const Filter filter);
extern D3D12_TEXTURE_ADDRESS_MODE GFXToD3D12TextureAddressMode(const TextureAddressMode mode);
extern Format DXGIFormatToGFX(const DXGI_FORMAT format);
extern DXGI_FORMAT GFXToDXGIFormat(const Format format);
extern D3D12_COMMAND_LIST_TYPE GFXToD3D12CommandListType(const CommandBufferType type);
extern D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE GFXToD3D12RenderPassBeginType(const RenderPassLoadOp op);
extern D3D12_RENDER_PASS_ENDING_ACCESS_TYPE GFXToD3D12RenderPassEndType(const RenderPassStoreOp op);
extern D3D12_RESOURCE_STATES GFXToD3D12ResourceState(const MemoryState state);

}