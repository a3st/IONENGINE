// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "renderer/rhi/command_buffer.hpp"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine {

namespace renderer {

namespace rhi {

class CommandAllocator : public core::ref_counted_object {
public:

    CommandAllocator(ID3D12Device1* device);

    auto allocate(D3D12_COMMAND_LIST_TYPE const list_type) -> core::ref_ptr<CommandBuffer>;

    auto reset() -> void;

private:

    ID3D12Device1* device;

    std::unordered_map<D3D12_COMMAND_LIST_TYPE, winrt::com_ptr<ID3D12CommandAllocator>> allocators;
};

}

}

}