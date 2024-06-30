// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "rhi.hpp"
#include "precompiled.h"

#ifdef IONENGINE_RHI_DIRECTX12
#include "rhi/dx12/rhi.hpp"
#elif IONENGINE_RHI_VULKAN
#include "rhi/vk/rhi.hpp"
#endif

namespace ionengine::rhi
{
    auto Device::create(RHICreateInfo const& createInfo) -> core::ref_ptr<Device>
    {
#ifdef IONENGINE_RHI_DIRECTX12
        return core::make_ref<DX12Device>(createInfo);
#elif IONENGINE_RHI_VULKAN
        return core::make_ref<VKDevice>(createInfo);
#endif
    }
} // namespace ionengine::rhi