// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "command_allocator.hpp"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine {

namespace renderer {

namespace rhi {

class UploadContext : public core::ref_counted_object {
public:

    UploadContext(
        ID3D12Device1* device, 
        ID3D12CommandQueue* queue, 
        ID3D12Fence* fence, 
        uint64_t& fence_value,
        CommandAllocator& allocator,
        size_t const size
    );

    auto upload(ID3D12Resource* dst, std::span<uint8_t const> const data) -> void;

private:

    ID3D12CommandQueue* queue;
    ID3D12Fence* fence;
    uint64_t* fence_value;
    CommandAllocator* allocator;
    winrt::com_ptr<ID3D12Heap> heap;
    winrt::com_ptr<ID3D12Resource> resource;
    uint64_t offset{0};
    size_t size;
};

}

}

}