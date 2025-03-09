// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"
#include "core/matrix.hpp"
#include "core/vector.hpp"

namespace ionengine::shadersys
{
    class HLSLCodeGen
    {
      public:
        HLSLCodeGen() = default;

        template <typename Type>
        auto property(std::string_view const inputName, std::string_view const semantic = "") -> void
        {
            std::string dataType;
            if constexpr (std::is_same_v<Type, uint32_t>)
            {
                dataType = "uint";
            }
            else if constexpr (std::is_same_v<Type, core::Vec4f>)
            {
                dataType = "float4";
            }
            else if constexpr (std::is_same_v<Type, core::Vec3f>)
            {
                dataType = "float3";
            }
            else if constexpr (std::is_same_v<Type, core::Vec2f>)
            {
                dataType = "float2";
            }
            else if constexpr (std::is_same_v<Type, core::Mat4f>)
            {
                dataType = "float4x4";
            }

            if (semantic.empty())
            {
                *stream << dataType << " " << inputName << "; ";
            }
            else
            {
                *stream << dataType << " " << inputName << " : " << semantic << "; ";
            }
        }

        template <typename Type>
        auto getHLSLStruct(std::string_view const structureName) -> std::string
        {
            std::stringstream ss;
            this->stream = &ss;
            ss << "struct " << structureName << " { ";
            Type::toHLSL(*this);
            ss << "};";
            return ss.str();
        }

        template <typename Type>
        auto getHLSLConstBuffer(std::string_view const structureName, uint32_t const r, uint32_t const s) -> std::string
        {
            std::stringstream ss;
            this->stream = &ss;
            ss << "cbuffer " << structureName << " : register(b" << r << ", space" << s << ") { ";
            Type::toHLSL(*this);
            ss << "};";
            return ss.str();
        }

      private:
        std::stringstream* stream;
    };
} // namespace ionengine::shadersys