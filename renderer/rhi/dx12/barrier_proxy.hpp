// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "buffer.hpp"
#include "texture.hpp"

namespace ionengine::renderer::rhi
{
    class BarrierProxy
    {
      public:
        BarrierProxy(ID3D12GraphicsCommandList4* command_list);

        ~BarrierProxy();

        auto add_transition(DX12Buffer& buffer, D3D12_RESOURCE_STATES const state) -> void;

        auto add_transition(DX12Texture& texture, D3D12_RESOURCE_STATES const state) -> void;

      private:
        ID3D12GraphicsCommandList4* command_list;
        std::vector<D3D12_RESOURCE_BARRIER> barriers;
    };
} // namespace ionengine::renderer::rhi