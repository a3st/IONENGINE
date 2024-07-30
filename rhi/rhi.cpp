// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "rhi.hpp"
#include "precompiled.h"

#ifdef IONENGINE_RHI_DIRECTX12
#include "rhi/dx12/dx12.hpp"
#elif IONENGINE_RHI_VULKAN
#include "rhi/vulkan/vk.hpp"
#endif

namespace ionengine::rhi
{
    auto sizeof_VertexFormat(VertexFormat const format) -> uint32_t
    {
        switch (format)
        {
            case VertexFormat::Float4:
                return sizeof(float) * 4;
            case VertexFormat::Float4x4:
                return sizeof(float) * 16;
            case VertexFormat::Float3:
                return sizeof(float) * 3;
            case VertexFormat::Float3x3:
                return sizeof(float) * 9;
            case VertexFormat::Float2:
                return sizeof(float) * 2;
            case VertexFormat::Float2x2:
                return sizeof(float) * 4;
            case VertexFormat::Float:
                return sizeof(float);
            case VertexFormat::Uint:
                return sizeof(uint32_t);
            default:
                return 0;
        }
    }

    auto Device::create(RHICreateInfo const& createInfo) -> core::ref_ptr<Device>
    {
#ifdef IONENGINE_RHI_DIRECTX12
        return core::make_ref<DX12Device>(createInfo);
#elif IONENGINE_RHI_VULKAN
        return core::make_ref<VKDevice>(createInfo);
#elif IONENGINE_RHI_GL
        return core::make_ref<GLDevice>(createInfo);
#else
#error rhi backend is not defined
        return nullptr;
#endif
    }
} // namespace ionengine::rhi