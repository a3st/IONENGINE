// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/matrix.hpp"
#include "core/vector.hpp"
#include "shader.hpp"
#include "upload_manager.hpp"

namespace ionengine
{
    class Material : public core::ref_counted_object
    {
      public:
        Material(rhi::RHI& RHI, core::ref_ptr<Shader> shader);

        auto getShader() const -> core::ref_ptr<Shader>;

        auto getEffectDataBuffer() const -> core::ref_ptr<rhi::Buffer>;

        auto updateEffectDataBuffer(internal::UploadManager* uploadManager) -> void;

        inline static core::ref_ptr<Material> baseSurfaceMaterial;

      private:
        core::ref_ptr<Shader> shader;
        core::ref_ptr<rhi::Buffer> effectDataBuffer;
        std::vector<uint8_t> effectDataRawBuffer;
        bool wasChanged;

      public:
        auto setValue(std::string_view const paramName, core::Mat4f const& value) -> void;

        auto setValue(std::string_view const paramName, core::Vec4f const& value) -> void;

        auto setValue(std::string_view const paramName, core::Vec3f const& value) -> void;

        auto setValue(std::string_view const paramName, core::Vec2f const& value) -> void;

        auto setValue(std::string_view const paramName, float const value) -> void;

        auto setValue(std::string_view const paramName, uint32_t const value) -> void;

        auto setValue(std::string_view const paramName, bool const value) -> void;
    };
} // namespace ionengine