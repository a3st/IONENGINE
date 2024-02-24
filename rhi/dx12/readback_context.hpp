// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "buffer.hpp"
#include "command_allocator.hpp"
#include "core/ref_ptr.hpp"
#include "memory_allocator.hpp"
#define NOMINMAX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine::renderer::rhi
{
    class ReadbackContext : public core::ref_counted_object
    {
      public:
        ReadbackContext(ID3D12Device4* device, ID3D12CommandQueue* queue, ID3D12Fence* fence, HANDLE fence_event,
                      uint64_t& fence_value, size_t const size);

        auto read(core::ref_ptr<Texture> src, std::vector<std::vector<uint8_t>>& data) -> void;

      private:
        ID3D12Device4* device;
        ID3D12CommandQueue* queue;
        ID3D12Fence* fence;
        HANDLE fence_event;
        uint64_t* fence_value;
        core::ref_ptr<DX12MemoryAllocator> memory_allocator;
        core::ref_ptr<CommandAllocator> command_allocator;
        core::ref_ptr<DX12Buffer> buffer;
    };
} // namespace ionengine::renderer::rhi