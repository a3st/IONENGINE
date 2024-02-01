// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "core/exception.hpp"
#include "future.hpp"
#include "precompiled.h"
#include "utils.hpp"

namespace ionengine::renderer::rhi
{
    DX12FutureImpl::DX12FutureImpl(ID3D12CommandQueue* queue, ID3D12Fence* fence, HANDLE fence_event,
                                   uint64_t const fence_value)
        : queue(queue), fence(fence), fence_event(fence_event), fence_value(fence_value)
    {
    }

    auto DX12FutureImpl::get_result() const -> bool
    {
        return fence->GetCompletedValue() >= fence_value;
    }

    auto DX12FutureImpl::wait() -> void
    {
        if (fence->GetCompletedValue() < fence_value)
        {
            THROW_IF_FAILED(fence->SetEventOnCompletion(fence_value, fence_event));
            ::WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
        }
    }
} // namespace ionengine::renderer::rhi