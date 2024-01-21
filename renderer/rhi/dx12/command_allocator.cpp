// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "precompiled.h"
#include "command_allocator.hpp"
#include "command_buffer.hpp"
#include "utils.hpp"
#include "core/exception.hpp"

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

CommandAllocator::CommandAllocator(ID3D12Device4* device) : device(device) {

    auto list_types = std::array<D3D12_COMMAND_LIST_TYPE, 4> {
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
        chunks.try_emplace(list_type);
    }
}

auto CommandAllocator::allocate(D3D12_COMMAND_LIST_TYPE const list_type) -> core::ref_ptr<CommandBuffer> {

    assert((chunks.find(list_type) != chunks.end()) && "Required list type not found when allocating command buffer");
    
    core::ref_ptr<CommandBuffer> buffer{nullptr};

    for(auto& chunk : chunks[list_type]) {
        if(1 > chunk.size - chunk.offset) {
            continue;
        }

        uint32_t const offset = chunk.offset;
        chunk.offset += 1;

        buffer = chunk.buffers[offset];
    }

    if(!buffer) {
        create_chunk(list_type);

        auto& chunk = chunks[list_type].back();

        uint32_t const offset = chunk.offset;
        chunk.offset += 1;

        buffer = chunk.buffers[offset];
    }

    return buffer;
}

auto CommandAllocator::reset() -> void {

    auto list_types = std::array<D3D12_COMMAND_LIST_TYPE, 4> {
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        D3D12_COMMAND_LIST_TYPE_COPY,
        D3D12_COMMAND_LIST_TYPE_COMPUTE,
        D3D12_COMMAND_LIST_TYPE_BUNDLE
    };

    for(auto const& list_type : list_types) {
        for(auto& chunk : chunks[list_type]) {
            if(chunk.offset == 0) {
                continue;
            }

            THROW_IF_FAILED(allocators[list_type]->Reset());
            chunk.offset = 0;
        }
    }
}

auto CommandAllocator::create_chunk(D3D12_COMMAND_LIST_TYPE const list_type) -> void {

    std::vector<core::ref_ptr<CommandBuffer>> buffers;

    uint32_t alloc_size = 0;
    switch(list_type) {
        case D3D12_COMMAND_LIST_TYPE_DIRECT: {
            alloc_size = CHUNK_COMMAND_LIST_DIRECT_MAX;
        } break;
        case D3D12_COMMAND_LIST_TYPE_COPY: {
            alloc_size = CHUNK_COMMAND_LIST_COPY_MAX;
        } break;
        case D3D12_COMMAND_LIST_TYPE_COMPUTE: {
            alloc_size = CHUNK_COMMAND_LIST_COMPUTE_MAX;
        } break;
        default: {
            assert(false && "Unsupported command buffer type for creation chunk");
        } break;
    }

    for(uint32_t i = 0; i < alloc_size; ++i) {
        winrt::com_ptr<ID3D12GraphicsCommandList4> command_list;
        THROW_IF_FAILED(device->CreateCommandList1(
            0,
            list_type,
            D3D12_COMMAND_LIST_FLAG_NONE,
            __uuidof(ID3D12GraphicsCommandList4),
            command_list.put_void()
        ));

        buffers.emplace_back(core::make_ref<DX12CommandBuffer>(device, allocators[list_type].get(), command_list, list_type));
    }

    auto chunk = Chunk {
        .buffers = std::move(buffers),
        .offset = 0,
        .size = alloc_size
    };
    chunks[list_type].emplace_back(chunk);
}
