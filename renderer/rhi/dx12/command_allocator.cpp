// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "precompiled.h"
#include "command_allocator.hpp"
#include "utils.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

CommandAllocator::CommandAllocator(ID3D12Device1* device) : device(device) {

    auto list_types = std::vector<D3D12_COMMAND_LIST_TYPE> {
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_LIST_TYPE_COPY,
        D3D12_COMMAND_LIST_TYPE_COMPUTE,
        D3D12_COMMAND_LIST_TYPE_BUNDLE
    };

    for(auto const& list_type : list_types) {
        winrt::com_ptr<ID3D12CommandAllocator> allocator;
        THROW_IF_FAILED(device->CreateCommandAllocator(
            list_type, 
            __uuidof(ID3D12CommandAllocator), 
            allocator.put_void()
        ));
        allocators.emplace(list_type, allocator);
    }
}

auto CommandAllocator::allocate(D3D12_COMMAND_LIST_TYPE const list_type) -> core::ref_ptr<CommandBuffer> {

    return {nullptr};
}

auto CommandAllocator::reset() -> void {


}