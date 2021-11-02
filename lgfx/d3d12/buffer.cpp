

#include "../precompiled.h"
#include "buffer.h"
#include "device.h"

using namespace lgfx;

Buffer::Buffer(Device* device, MemoryPool* pool, const BufferDesc& desc) : pool_(pool), desc_(desc) {

    resource_desc_.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc_.Width = desc.size;
    resource_desc_.Height = 1;
    resource_desc_.MipLevels = 1;
    resource_desc_.DepthOrArraySize = 1;
    resource_desc_.SampleDesc.Count = 1;
    resource_desc_.Format = DXGI_FORMAT_UNKNOWN;
    if(desc_.flags & BufferFlags::kConstantBuffer) {
        resource_desc_.Width = (desc.size + 255) & ~255;
    }

    initial_state_ = D3D12_RESOURCE_STATE_GENERIC_READ;

    D3D12_RESOURCE_ALLOCATION_INFO alloc_info = device->device_->GetResourceAllocationInfo(0, 1, &resource_desc_);
    alloc_info_ = pool->Allocate(alloc_info.SizeInBytes);
    
    THROW_IF_FAILED(device->device_->CreatePlacedResource(alloc_info_.heap->heap_.Get(), alloc_info_.offset, &resource_desc_, initial_state_, nullptr, __uuidof(ID3D12Resource), reinterpret_cast<void**>(resource_.GetAddressOf())));
}

Buffer::~Buffer() {

    if(pool_) {
        pool_->Deallocate(alloc_info_);
    }
}