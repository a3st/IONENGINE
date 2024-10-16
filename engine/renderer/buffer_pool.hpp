// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class BufferPool : public core::ref_counted_object
    {
      public:
        virtual auto reset() -> void = 0;

      protected:
        virtual auto allocateMemory(size_t const size) -> core::weak_ptr<rhi::Buffer> = 0;
    };

    class ConstantBufferPool : public BufferPool
    {
      public:
        ConstantBufferPool(rhi::Device& device);

        template <typename Type>
        auto allocateWrite(rhi::CopyContext& context, Type const& object) -> core::weak_ptr<rhi::Buffer>
        {
            auto buffer = allocateMemory(sizeof(Type));
            if (!buffer)
            {
                throw core::runtime_error("An error occurred while allocating memory");
            }
            context.barrier(buffer.get(), rhi::ResourceState::Common, rhi::ResourceState::CopyDest);
            context.writeBuffer(buffer.get(), std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&object), sizeof(Type)));
            context.barrier(buffer.get(), rhi::ResourceState::CopyDest, rhi::ResourceState::Common);
            return buffer;
        }

      protected:
        auto allocateMemory(size_t const size) -> core::weak_ptr<rhi::Buffer> override;

      public:
        auto reset() -> void override;

      private:
        rhi::Device* device;

        using list_of_buffer_t = std::list<core::ref_ptr<rhi::Buffer>>;

        list_of_buffer_t buffers;

        struct Entry
        {
            std::list<list_of_buffer_t::iterator> elements;
            std::list<list_of_buffer_t::iterator>::iterator cur;
        };

        std::map<size_t, Entry> buckets;
    };
} // namespace ionengine