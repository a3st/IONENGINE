// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "precompiled.h"
#include "core/exception.hpp"
#include "platform/window.hpp"
#include "renderer/rhi/dx12/device.hpp"

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::rhi;

auto Device::create(BackendType const backend_type, platform::Window const& window) -> core::ref_ptr<Device> {

    switch(backend_type) {
        case BackendType::DirectX12: return core::make_ref<DX12Device>(window);
        default: {
            throw core::Exception("Selected backend is not supported");
        } break;
    }
}