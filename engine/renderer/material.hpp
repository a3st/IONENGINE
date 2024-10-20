// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "shader.hpp"

namespace ionengine
{
    enum class MaterialDomain
    {
        Surface
    };

    enum class MaterialBlend
    {
        Opaque,
        Translucent
    };

    class Material : public core::ref_counted_object
    {
      public:
        Material(rhi::Device& device, MaterialDomain const domain, MaterialBlend const blend,
                 core::ref_ptr<Shader> shader);

        template <typename Type>
        auto setValue(std::string_view const parameter, Type const& value) -> void
        {
            auto result = parameterNames.find(std::string(parameter));
            if (result == parameterNames.end())
            {
            }

            if constexpr (std::is_same_v<Type, math::Color>)
            {
                if (result->second.type != shadersys::fx::ElementType::Float4)
                {
                }

                std::memcpy(rawBuffer.data(), value.data(), sizeof(math::Color));
            }
        }

        auto update(rhi::CopyContext& copyContext) -> void;

        auto getBuffer() -> core::ref_ptr<rhi::Buffer>;

        auto getShader(bool isSkin) -> core::ref_ptr<ShaderVariant>;

      private:
        rhi::CopyContext* copyContext;

        struct ParameterData
        {
            uint64_t offset;
            shadersys::fx::ElementType type;
        };

        core::ref_ptr<rhi::Buffer> constantBuffer;
        std::vector<uint8_t> rawBuffer;
        bool isUpdated;

        core::ref_ptr<Shader> shader;

        std::unordered_map<std::string, ParameterData> parameterNames;
    };
} // namespace ionengine