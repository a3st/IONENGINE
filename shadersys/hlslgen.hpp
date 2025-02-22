// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"
#include "math/matrix.hpp"
#include "math/vector.hpp"

namespace ionengine::shadersys
{
    template <typename Type>
    struct cbuffer_t
    {
        using element_t = Type;
    };

    struct sampler_t
    {
    };

    namespace internal
    {
        template <typename>
        struct is_cbuffer_t : std::false_type
        {
        };

        template <typename T>
        struct is_cbuffer_t<cbuffer_t<T>> : std::true_type
        {
        };
    } // namespace internal

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
            else if constexpr (std::is_same_v<Type, math::Vec4f>)
            {
                dataType = "float4";
            }
            else if constexpr (std::is_same_v<Type, math::Vec3f>)
            {
                dataType = "float3";
            }
            else if constexpr (std::is_same_v<Type, math::Vec2f>)
            {
                dataType = "float2";
            }
            else if constexpr (std::is_same_v<Type, math::Mat4f>)
            {
                dataType = "float4x4";
            }
            else if constexpr (internal::is_cbuffer_t<Type>::value)
            {
                std::string typeName = typeid(Type::element_t).name();
                this->getClassName(typeName);
                dataType = "cbuffer_t<" + typeName + ">";
            }
            else if constexpr (std::is_same_v<Type, sampler_t>)
            {
                dataType = "sampler_t";
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
        auto getHLSLConstBuffer(std::string_view const structureName, std::string_view const variableName,
                                uint32_t const r, uint32_t const s) -> std::string
        {
            std::stringstream ss;
            this->stream = &ss;
            ss << "struct " << structureName << " { ";
            Type::toHLSL(*this);
            ss << "};\nConstantBuffer<" << structureName << "> " << variableName << " : register(b" << r << ", space"
               << s << ");";
            return ss.str();
        }

      private:
        std::stringstream* stream;

        auto getClassName(std::string& source) -> void
        {
            source.erase(source.begin(), std::find_if(source.rbegin(), source.rend(), [](unsigned char ch) {
                                             return ch == ':';
                                         }).base());
        }
    };
} // namespace ionengine::shadersys