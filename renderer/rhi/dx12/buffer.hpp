// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "descriptor_allocator.hpp"
#include "memory_allocator.hpp"
#include "renderer/rhi/buffer.hpp"

namespace ionengine::renderer::rhi
{
    inline uint32_t constexpr DX12_CONSTANT_BUFFER_SIZE_ALIGNMENT = 256;

    class DX12Buffer : public Buffer
    {
      public:
        DX12Buffer(ID3D12Device1* device, DX12MemoryAllocator& memory_allocator,
                   DescriptorAllocator* descriptor_allocator, size_t const size, BufferUsageFlags const flags);

        ~DX12Buffer();

        auto get_size() -> size_t override
        {
            return size;
        }

        auto get_flags() -> BufferUsageFlags override
        {
            return flags;
        }

        auto get_resource() -> ID3D12Resource*
        {
            return resource.get();
        }

        auto get_descriptor(BufferUsage const usage) const -> DescriptorAllocation const&
        {
            return descriptor_allocations.at(usage);
        }

        auto get_resource_state() const -> D3D12_RESOURCE_STATES
        {
            return resource_state;
        }

        auto set_resource_state(D3D12_RESOURCE_STATES const state) -> void
        {
            resource_state = state;
        }

      private:
        DX12MemoryAllocator* memory_allocator;
        DescriptorAllocator* descriptor_allocator;
        winrt::com_ptr<ID3D12Resource> resource;
        D3D12_RESOURCE_STATES resource_state;
        MemoryAllocation memory_allocation;
        std::unordered_map<BufferUsage, DescriptorAllocation> descriptor_allocations;
        size_t size;
        BufferUsageFlags flags;
    };
} // namespace ionengine::renderer::rhi