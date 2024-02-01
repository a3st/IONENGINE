// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/rhi/future.hpp"
#define NOMINMAX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine::renderer::rhi
{
    class DX12FutureImpl : public FutureImpl
    {
      public:
        DX12FutureImpl(ID3D12CommandQueue* queue, ID3D12Fence* fence, HANDLE fence_event, uint64_t const fence_value);

        auto get_result() const -> bool override;

        auto wait() -> void override;

      private:
        ID3D12CommandQueue* queue;
        ID3D12Fence* fence;
        HANDLE fence_event;
        uint64_t fence_value;
    };
} // namespace ionengine::renderer::rhi