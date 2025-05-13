// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "rhi.hpp"
#include "precompiled.h"

#ifdef IONENGINE_RHI_DIRECTX12
#include "dx12/dx12.hpp"
#elif IONENGINE_RHI_VULKAN
#include "vulkan/vk.hpp"
#endif

namespace ionengine::rhi
{
    auto sizeof_VertexFormat(VertexFormat const format) -> size_t
    {
        switch (format)
        {
            case VertexFormat::RGBA32_FLOAT:
                return sizeof(float) * 4;
            case VertexFormat::RGBA32_SINT:
                return sizeof(int32_t) * 4;
            case VertexFormat::RGBA32_UINT:
                return sizeof(uint32_t) * 4;
            case VertexFormat::RGB32_FLOAT:
                return sizeof(float) * 3;
            case VertexFormat::RGB32_SINT:
                return sizeof(int32_t) * 3;
            case VertexFormat::RGB32_UINT:
                return sizeof(uint32_t) * 3;
            case VertexFormat::RG32_FLOAT:
                return sizeof(float) * 2;
            case VertexFormat::RG32_SINT:
                return sizeof(int32_t) * 2;
            case VertexFormat::RG32_UINT:
                return sizeof(uint32_t) * 2;
            case VertexFormat::R32_FLOAT:
                return sizeof(float);
            case VertexFormat::R32_SINT:
                return sizeof(int32_t);
            case VertexFormat::R32_UINT:
                return sizeof(uint32_t);
            default:
                return 0;
        }
    }

    auto RHI::create(RHICreateInfo const& rhiCreateInfo, std::optional<SwapchainCreateInfo> const swapchainCreateInfo)
        -> core::ref_ptr<RHI>
    {
#ifdef IONENGINE_RHI_DIRECTX12
        return core::make_ref<DX12RHI>(rhiCreateInfo, swapchainCreateInfo);
#elif IONENGINE_RHI_VULKAN
        return core::make_ref<VKRHI>(createInfo, swapchainCreateInfo);
#else
#error rhi backend is not defined
        return nullptr;
#endif
    }
} // namespace ionengine::rhi