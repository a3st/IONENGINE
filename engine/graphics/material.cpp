// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "material.hpp"
#include "precompiled.h"

namespace ionengine
{
    Material::Material(rhi::RHI& RHI, core::ref_ptr<Shader> shader) : shader(shader)
    {
        auto effectDataResult = shader->getBindings().find("EFFECT_DATA");

        if (effectDataResult != shader->getBindings().end() && effectDataResult->second.size > 0)
        {
            rhi::BufferCreateInfo const bufferCreateInfo{
                .size = effectDataResult->second.size,
                .flags = (rhi::BufferUsageFlags)(rhi::BufferUsage::Vertex | rhi::BufferUsage::CopyDest)};

            effectDataBuffer = RHI.createBuffer(bufferCreateInfo);

            effectDataRawBuffer.resize(effectDataResult->second.size);
        }
    }

    auto Material::getShader() const -> core::ref_ptr<Shader>
    {
        return shader;
    }

    auto Material::setValue(std::string_view const paramName, core::Mat4f const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, value.data(), sizeof(core::Mat4f));
    }

    auto Material::setValue(std::string_view const paramName, core::Vec4f const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, value.data(), sizeof(core::Vec4f));
    }

    auto Material::setValue(std::string_view const paramName, core::Vec3f const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, value.data(), sizeof(core::Vec3f));
    }

    auto Material::setValue(std::string_view const paramName, core::Vec2f const& value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, value.data(), sizeof(core::Vec2f));
    }

    auto Material::setValue(std::string_view const paramName, float const value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, &value, sizeof(float));
    }

    auto Material::setValue(std::string_view const paramName, uint32_t const value) -> void
    {
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, &value, sizeof(uint32_t));
    }

    auto Material::setValue(std::string_view const paramName, bool const value) -> void
    {
        uint32_t const convValue = value;
        uint64_t const paramOffset = shader->getBindings().at("EFFECT_DATA").elements.at(std::string(paramName));
        std::memcpy(effectDataRawBuffer.data() + paramOffset, &convValue, sizeof(uint32_t));
    }
} // namespace ionengine