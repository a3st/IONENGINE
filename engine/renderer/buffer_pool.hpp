// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

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

      protected:
        auto allocateMemory(size_t const size) -> core::weak_ptr<rhi::Buffer> override;

      public:
        auto reset() -> void override;

      private:
        rhi::Device* device;

        using list_of_buffer_t = std::list<core::ref_ptr<rhi::Buffer>>;

        list_of_buffer_t buffers;
        list_of_buffer_t::iterator cur;
    };
} // namespace ionengine