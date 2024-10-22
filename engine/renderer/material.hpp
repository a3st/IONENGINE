// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

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
                throw core::runtime_error("An error occurred while setting a value to an invalid variable");
            }

            if constexpr (std::is_same_v<Type, math::Color>)
            {
                if (result->second.type != shadersys::fx::ElementType::Float4)
                {
                    throw core::runtime_error(
                        "An error occurred while setting a value to an variable with different type");
                }

                std::memcpy(rawBuffer.data(), value.data(), sizeof(math::Color));
            }
        }

        auto update(rhi::CopyContext& copyContext) -> void;

        auto getDomain() const -> MaterialDomain;

        auto getBlend() const -> MaterialBlend;

        auto getBuffer() const -> core::ref_ptr<rhi::Buffer>;

        auto getShader(bool const isSkin) -> core::ref_ptr<rhi::Shader>;

      private:
        struct ParameterData
        {
            uint64_t offset;
            shadersys::fx::ElementType type;
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