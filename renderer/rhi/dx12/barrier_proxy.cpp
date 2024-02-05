// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "barrier_proxy.hpp"
#include "precompiled.h"

namespace ionengine::renderer::rhi
{
    BarrierProxy::BarrierProxy(ID3D12GraphicsCommandList4* command_list) : command_list(command_list)
    {
    }

    BarrierProxy::~BarrierProxy()
    {
        if (!barriers.empty())
        {
            command_list->ResourceBarrier(static_cast<uint32_t>(barriers.size()), barriers.data());
            barriers.clear();
        }
    }

    auto BarrierProxy::add_transition(DX12Buffer& buffer, D3D12_RESOURCE_STATES const state) -> void
    {
        if (buffer.get_flags() & BufferUsage::MapWrite)
        {
            return;
        }

        if (buffer.get_resource_state() != state)
        {
            auto d3d12_resource_barrier = D3D12_RESOURCE_BARRIER{};
            d3d12_resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            d3d12_resource_barrier.Transition.StateBefore = buffer.get_resource_state();
            d3d12_resource_barrier.Transition.StateAfter = state;
            d3d12_resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            d3d12_resource_barrier.Transition.pResource = buffer.get_resource();

            buffer.set_resource_state(state);
            barriers.emplace_back(std::move(d3d12_resource_barrier));
        }
    }

    auto BarrierProxy::add_transition(DX12Texture& texture, D3D12_RESOURCE_STATES const state) -> void
    {
        if (texture.get_resource_state() != state)
        {
            auto d3d12_resource_barrier = D3D12_RESOURCE_BARRIER{};
            d3d12_resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            d3d12_resource_barrier.Transition.StateBefore = texture.get_resource_state();
            d3d12_resource_barrier.Transition.StateAfter = state;
            d3d12_resource_barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
            d3d12_resource_barrier.Transition.pResource = texture.get_resource();

            texture.set_resource_state(state);
            barriers.emplace_back(std::move(d3d12_resource_barrier));
        }
    }
} // namespace ionengine::renderer::rhi