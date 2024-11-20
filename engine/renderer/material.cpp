// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "material.hpp"
#include "precompiled.h"

namespace ionengine
{
    Material::Material(rhi::Device& device, core::ref_ptr<Shader> shader) : isUpdated(false), shader(shader)
    {
        auto result = shader->getStructureNames().find("MATERIAL_DATA");
        if (result != shader->getStructureNames().end())
        {
            throw core::runtime_error("Material is not supported by the shader system");
        }

        rhi::BufferCreateInfo bufferCreateInfo{.size = result->second.size,
                                               .flags = (rhi::BufferUsageFlags)rhi::BufferUsage::ConstantBuffer};
        buffer = device.createBuffer(bufferCreateInfo);

        rawBuffer.resize(result->second.size);

        uint64_t offset = 0;
        for (auto const& element : result->second.elements)
        {
            size_t const elementSize = asset::fx::sizeof_ElementType(element.type);

            ParameterData parameterData{.offset = offset, .type = element.type};
            parameterNames[element.name] = std::move(parameterData);
            offset += elementSize;
        }
    }

    auto Material::update(rhi::CopyContext& copyContext) -> void
    {
        if (!isUpdated)
        {
            copyContext.writeBuffer(buffer, rawBuffer);

            isUpdated = true;
        }
    }

    auto Material::getDomain() const -> MaterialDomain
    {
        return domain;
    }

    auto Material::getBlend() const -> MaterialBlend
    {
        return blend;
    }

    auto Material::getBuffer() const -> core::ref_ptr<rhi::Buffer>
    {
        return buffer;
    }

    auto Material::getShader(bool const enableSkinningFeature) -> core::ref_ptr<rhi::Shader>
    {
        uint32_t flags = shader->getPermutationNames().find("BASE")->second;
        if (enableSkinningFeature)
        {
            flags |= shader->getPermutationNames().find("FEATURE_SKINNING")->second;
        }
        return shader->getShader(flags);
    }
} // namespace ionengine