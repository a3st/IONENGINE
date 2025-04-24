// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "material.hpp"
#include "precompiled.h"
#include "upload_manager.hpp"

namespace ionengine
{
    Material::Material(rhi::RHI& RHI, uint32_t const frameCount, core::ref_ptr<Shader> const& shader)
        : shader(shader), frameCount(frameCount)
    {
        auto const& effectData = shader->getBindings().at("EFFECT_DATA");

        for (uint32_t const i : std::views::iota(0u, frameCount))
        {
            rhi::BufferCreateInfo const bufferCreateInfo{
                .size = effectData.size > 0 ? effectData.size : 256,
                .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::ConstantBuffer | rhi::BufferUsage::CopyDest)};
            effectDataBuffers.emplace_back(RHI.createBuffer(bufferCreateInfo));
        }

        effectDataRawBuffer.resize(effectData.size);
        isNeedUpdates.resize(frameCount, true);
    }

    auto Material::getShader() const -> core::ref_ptr<Shader>
    {
        return shader;
    }

    auto Material::getEffectDataBuffer(uint32_t const frameIndex) const -> core::ref_ptr<rhi::Buffer>
    {
        return effectDataBuffers[frameIndex];
    }

    auto Material::updateEffectDataBuffer(UploadManager& uploadManager, uint32_t const frameIndex) -> void
    {
        if (!isNeedUpdates[frameIndex])
        {
            return;
        }

        UploadBufferInfo const uploadBufferInfo{
            .buffer = effectDataBuffers[frameIndex], .offset = 0, .dataBytes = effectDataRawBuffer};
        uploadManager.uploadBuffer(uploadBufferInfo);

        isNeedUpdates[frameIndex] = false;
    }

    auto Material::setValue(std::string_view const paramName, core::ref_ptr<Image> const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        uint32_t const descriptor = value->getTexture()->getDescriptorOffset(rhi::TextureUsage::ShaderResource);
        std::memcpy(effectDataRawBuffer.data() + paramOffset, &descriptor, sizeof(uint32_t));
        std::fill(isNeedUpdates.begin(), isNeedUpdates.end(), true);
    }

    auto Material::setValue(std::string_view const paramName, core::Mat4f const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, value.data(), sizeof(core::Mat4f));
        std::fill(isNeedUpdates.begin(), isNeedUpdates.end(), true);
    }

    auto Material::setValue(std::string_view const paramName, core::Vec4f const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, value.data(), sizeof(core::Vec4f));
        std::fill(isNeedUpdates.begin(), isNeedUpdates.end(), true);
    }

    auto Material::setValue(std::string_view const paramName, core::Vec3f const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, value.data(), sizeof(core::Vec3f));
        std::fill(isNeedUpdates.begin(), isNeedUpdates.end(), true);
    }

    auto Material::setValue(std::string_view const paramName, core::Vec2f const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, value.data(), sizeof(core::Vec2f));
        std::fill(isNeedUpdates.begin(), isNeedUpdates.end(), true);
    }

    auto Material::setValue(std::string_view const paramName, float const value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, &value, sizeof(float));
        std::fill(isNeedUpdates.begin(), isNeedUpdates.end(), true);
    }

    auto Material::setValue(std::string_view const paramName, uint32_t const value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, &value, sizeof(uint32_t));
        std::fill(isNeedUpdates.begin(), isNeedUpdates.end(), true);
    }

    auto Material::setValue(std::string_view const paramName, bool const value) -> void
    {
        uint32_t const convValue = value;
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, &convValue, sizeof(uint32_t));
        std::fill(isNeedUpdates.begin(), isNeedUpdates.end(), true);
    }
} // namespace ionengine