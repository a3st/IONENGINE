// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "material.hpp"
#include "precompiled.h"

namespace ionengine
{
    Material::Material(rhi::Device& device, MaterialDomain const domain, MaterialBlend const blend,
                       core::ref_ptr<Shader> shader)
        : isUpdated(false), shader(shader)
    {
        if (!shader->getMaterialData())
        {
        }

        auto materialData = shader->getMaterialData().value();

        rhi::BufferCreateInfo bufferCreateInfo{.size = materialData.size,
                                               .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer};
        constantBuffer = device.createBuffer(bufferCreateInfo);

        rawBuffer.resize(materialData.size);

        uint64_t offset = 0;
        for (auto const& element : materialData.elements)
        {
            size_t const elementSize = shadersys::fx::sizeof_ElementType(element.type);

            ParameterData parameterData{.offset = offset, .type = element.type};
            parameterNames[element.name] = std::move(parameterData);
            offset += elementSize;
        }
    }

    auto Material::update(rhi::CopyContext& copyContext) -> void
    {
        if (!isUpdated)
        {
            copyContext.writeBuffer(constantBuffer, rawBuffer);

            isUpdated = true;
        }
    }

    auto Material::getBuffer() -> core::ref_ptr<rhi::Buffer>
    {
        return constantBuffer;
    }

    auto Material::getShader(bool isSkin) -> core::ref_ptr<ShaderVariant>
    {
        uint32_t flags = shader->getFlagsByName("BASE");
        return shader->getVariant(flags);
    }
} // namespace ionengine