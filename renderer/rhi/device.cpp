// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/exception.hpp"
#include "platform/window.hpp"
#include "precompiled.h"
#include "renderer/rhi/dx12/device.hpp"

namespace ionengine::renderer::rhi
{
    auto Device::create(BackendType const backend_type, platform::Window const& window) -> core::ref_ptr<Device>
    {
        switch (backend_type)
        {
            case BackendType::DirectX12:
                return core::make_ref<DX12Device>(window);
            default: {
                throw core::Exception("Selected graphics backend is not supported");
            }
            break;
        }
    }
} // namespace ionengine::renderer::rhi