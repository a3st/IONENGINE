// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/error.hpp"
#include "core/matrix.hpp"
#include "core/vector.hpp"
#include "fx.hpp"

namespace ionengine::shadersys
{
    class HLSLCodeGenerator
    {
      public:
        HLSLCodeGenerator() = default;

        HLSLCodeGenerator(std::unordered_map<std::string, int32_t> const& defineValues)
            : _stream(nullptr), _defineValues(defineValues), _structureElements(nullptr), _structureSize(nullptr)
        {
        }

        auto defined(std::string_view const defineName) const -> bool
        {
            return true;
        }

        template <typename Type>
        auto property(std::string_view const inputName, std::string_view const semantic = "") -> void
        {
            asset::fx::ElementType elementType;
            if constexpr (std::is_same_v<Type, uint32_t>)
            {
                elementType = asset::fx::ElementType::Uint;
            }
            else if constexpr (std::is_same_v<Type, core::Vec4f>)
            {
                elementType = asset::fx::ElementType::Float4;
            }
            else if constexpr (std::is_same_v<Type, core::Vec3f>)
            {
                elementType = asset::fx::ElementType::Float3;
            }
            else if constexpr (std::is_same_v<Type, core::Vec2f>)
            {
                elementType = asset::fx::ElementType::Float2;
            }
            else if constexpr (std::is_same_v<Type, core::Mat4f>)
            {
                elementType = asset::fx::ElementType::Float4x4;
            }
            else if constexpr (std::is_same_v<Type, core::Vec4u>)
            {
                elementType = asset::fx::ElementType::Uint4;
            }

            auto typeResult = core::serialize<core::serialize_oenum, std::ostringstream>(elementType);
            assert(typeResult.has_value() && "missing field in serializable_enum<asset::fx::ElementType>");

            if (semantic.empty())
            {
                *_stream << typeResult.value().str() << " " << inputName << "; ";
            }
            else
            {
                *_stream << typeResult.value().str() << " " << inputName << " : " << semantic << "; ";
            }

            if (!this->isDefaultSemantic(semantic))
            {
                asset::fx::StructureElementData structureElement{
                    .name = std::string(inputName),
                    .type = elementType,
                    .semantic = semantic.empty() ? std::nullopt : std::make_optional<std::string>(semantic)};
                _structureElements->emplace_back(std::move(structureElement));
                *_structureSize += sizeof_ElementType(elementType);
            }
        }

        auto getHLSLStruct(asset::fx::StructureData const& structure, std::string& shaderCode) -> void
        {
            std::ostringstream oss;
            oss << "struct " << structure.name << " { ";

            for (auto const& element : structure.elements)
            {
                auto typeResult = core::serialize<core::serialize_oenum, std::ostringstream>(element.type);
                assert(typeResult.has_value() && "missing field in serializable_enum<asset::fx::ElementType>");

                oss << typeResult.value().str() << " " << element.name << "; ";
            }

            oss << "};";
            shaderCode = oss.str();
        }

        template <typename Type>
        auto getHLSLStruct(std::string_view const structureName, std::string& shaderCode,
                           asset::fx::StructureData& structure) -> void
        {
            uint32_t structureSize = 0;
            _structureSize = &structureSize;

            std::vector<asset::fx::StructureElementData> structureElements;
            _structureElements = &structureElements;

            std::stringstream ss;
            _stream = &ss;
            ss << "struct " << structureName << " { ";
            Type::toHLSL(*this);
            ss << "};";
            shaderCode = ss.str();
            structure = asset::fx::StructureData{
                .name = std::string(structureName), .elements = std::move(structureElements), .size = structureSize};
        }

        template <typename Type>
        auto getHLSLConstBuffer(std::string_view const structureName, uint32_t const r, uint32_t const s,
                                std::string& shaderCode, asset::fx::StructureData& structure) -> void
        {
            uint32_t structureSize = 0;
            _structureSize = &structureSize;

            std::vector<asset::fx::StructureElementData> structureElements;
            _structureElements = &structureElements;

            std::stringstream ss;
            _stream = &ss;
            ss << "cbuffer " << structureName << " : register(b" << r << ", space" << s << ") { ";
            Type::toHLSL(*this);
            ss << "};";
            shaderCode = ss.str();
            structure = asset::fx::StructureData{
                .name = std::string(structureName), .elements = std::move(structureElements), .size = structureSize};
        }

        auto isDefaultSemantic(std::string_view const semantic) const -> bool
        {
            return semantic.find("SV_") != std::string_view::npos;
        }

      private:
        std::stringstream* _stream;
        std::unordered_map<std::string, int32_t> _defineValues;
        std::vector<asset::fx::StructureElementData>* _structureElements;
        uint32_t* _structureSize;
    };
} // namespace ionengine::shadersys