// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "math/vector.hpp"
#include "shader.hpp"

namespace ionengine
{
    enum class MaterialDomain
    {
        Surface,
        Screen
    };

    enum class MaterialBlend
    {
        Opaque,
        Translucent
    };

    class Material : public core::ref_counted_object
    {
      public:
        Material(rhi::Device& device, core::ref_ptr<Shader> shader);

        template <typename Type>
        auto setValue(std::string_view const parameter, Type const& value) -> void
        {
            auto result = parameterNames.find(std::string(parameter));
            if (result == parameterNames.end())
            {
                throw core::runtime_error("Parameter being passed was not found in the shader");
            }

            if constexpr (std::is_same_v<Type, math::Color> || std::is_same_v<Type, math::Vec4f>)
            {
                if (result->second.type != asset::fx::ElementType::Float4)
                {
                    throw core::runtime_error("Unsupported type passed to shader");
                }

                std::memcpy(rawBuffer.data(), value.data(), sizeof(Type));
            }
            else if constexpr (std::is_same_v<Type, math::Vec3f>)
            {
                if (result->second.type != asset::fx::ElementType::Float3)
                {
                    throw core::runtime_error("Unsupported type passed to shader");
                }

                std::memcpy(rawBuffer.data(), value.data(), sizeof(math::Vec3f));
            }
            else if constexpr (std::is_same_v<Type, math::Vec2f>)
            {
                if (result->second.type != asset::fx::ElementType::Float2)
                {
                    throw core::runtime_error("Unsupported type passed to shader");
                }

                std::memcpy(rawBuffer.data(), value.data(), sizeof(math::Vec2f));
            }
            else if constexpr (std::is_floating_point_v<Type>)
            {
                if (result->second.type != asset::fx::ElementType::Float)
                {
                    throw core::runtime_error("Unsupported type passed to shader");
                }

                std::memcpy(rawBuffer.data(), value.data(), sizeof(float));
            }
            else
            {
                throw core::runtime_error("Unsupported type passed to shader");
            }
        }

        auto update(rhi::CopyContext& copyContext) -> void;

        auto getDomain() const -> MaterialDomain;

        auto getBlend() const -> MaterialBlend;

        auto getBuffer() const -> core::ref_ptr<rhi::Buffer>;

        auto getShader(bool const enableSkinningFeature) -> core::ref_ptr<rhi::Shader>;

      private:
        struct ParameterData
        {
            uint64_t offset;
            asset::fx::ElementType type;
        };

        std::vector<uint8_t> rawBuffer;
        bool isUpdated;

        std::unordered_map<std::string, ParameterData> parameterNames;

        core::ref_ptr<Shader> shader;
        core::ref_ptr<rhi::Buffer> buffer;

        MaterialDomain domain;
        MaterialBlend blend;
    };
} // namespace ionengine