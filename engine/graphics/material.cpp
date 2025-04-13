// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "material.hpp"
#include "precompiled.h"
#include "upload_manager.hpp"

namespace ionengine
{
    Material::Material(rhi::RHI& RHI, uint32_t const frameCount, core::ref_ptr<Shader> shader)
        : shader(shader), wasChangedCount(0), frameCount(frameCount)
    {
        auto effectDataResult = shader->getBindings().find("EFFECT_DATA");

        if (effectDataResult != shader->getBindings().end() && effectDataResult->second.size > 0)
        {
            for (uint32_t const i : std::views::iota(0u, frameCount))
            {
                rhi::BufferCreateInfo const bufferCreateInfo{
                    .size = effectDataResult->second.size,
                    .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::ConstantBuffer | rhi::BufferUsage::CopyDest)};
                effectDataBuffers.emplace_back(RHI.createBuffer(bufferCreateInfo));
            }

            effectDataRawBuffer.resize(effectDataResult->second.size);
        }
    }

    auto Material::getShader() const -> core::ref_ptr<Shader>
    {
        return shader;
    }

    auto Material::getEffectDataBuffer(uint32_t const frameIndex) const -> core::ref_ptr<rhi::Buffer>
    {
        return effectDataBuffers[frameIndex];
    }

    auto Material::updateEffectDataBuffer(UploadManager* uploadManager, uint32_t const frameIndex) -> void
    {
        if (effectDataBuffers.empty())
        {
            return;
        }

        if (wasChangedCount != frameCount)
        {
            UploadBufferInfo const uploadBufferInfo{
                .buffer = effectDataBuffers[frameIndex], .offset = 0, .dataBytes = effectDataRawBuffer};
            uploadManager->uploadBuffer(uploadBufferInfo, [this]() -> void {});

            wasChangedCount++;
        }
    }

    auto Material::setValue(std::string_view const paramName, core::ref_ptr<Image> const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        uint32_t const descriptor = value->getTexture()->getDescriptorOffset(rhi::TextureUsage::ShaderResource);
        std::memcpy(effectDataRawBuffer.data() + paramOffset, &descriptor, sizeof(uint32_t));
        wasChangedCount = 0;
    }

    auto Material::setValue(std::string_view const paramName, core::Mat4f const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, value.data(), sizeof(core::Mat4f));
        wasChangedCount = 0;
    }

    auto Material::setValue(std::string_view const paramName, core::Vec4f const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, value.data(), sizeof(core::Vec4f));
        wasChangedCount = 0;
    }

    auto Material::setValue(std::string_view const paramName, core::Vec3f const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, value.data(), sizeof(core::Vec3f));
        wasChangedCount = 0;
    }

    auto Material::setValue(std::string_view const paramName, core::Vec2f const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, value.data(), sizeof(core::Vec2f));
        wasChangedCount = 0;
    }

    auto Material::setValue(std::string_view const paramName, float const value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, &value, sizeof(float));
        wasChangedCount = 0;
    }

    auto Material::setValue(std::string_view const paramName, uint32_t const value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, &value, sizeof(uint32_t));
        wasChangedCount = 0;
    }

    auto Material::setValue(std::string_view const paramName, bool const value) -> void
    {
        uint32_t const convValue = value;
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, &convValue, sizeof(uint32_t));
        wasChangedCount = 0;
    }
} // namespace ionengine