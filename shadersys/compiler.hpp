// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "math/matrix.hpp"
#include "shadersys/fx.hpp"

namespace ionengine::shadersys
{
    auto getAllVariants(std::span<std::string const> const permutations,
                        std::unordered_map<std::string, uint32_t> const& flags) -> std::vector<uint32_t>;

    namespace common
    {
#pragma pack(push, 1)
        struct TransformData
        {
            math::Matf modelViewProj;
            math::Matf inverseModelViewProj;
        };
#pragma pack(pop)

        inline asset::fx::ConstantData const transformConstantData{.name = "transformBuffer",
                                                                   .type = asset::fx::ElementType::Uint};
        inline asset::fx::StructureData const transformStructureData{
            .name = "TRANSFORM_DATA",
            .elements = {
                asset::fx::StructureElementData{.name = "modelViewProj", .type = asset::fx::ElementType::Float4x4},
                asset::fx::StructureElementData{.name = "inverseModelViewProj",
                                                .type = asset::fx::ElementType::Float4x4}}};

#pragma pack(push, 1)
        struct LightingData
        {
            uint32_t reserved;
        };
#pragma pack(pop)

        inline asset::fx::ConstantData const materialConstantData{.name = "materialBuffer",
                                                                  .type = asset::fx::ElementType::Uint};
    } // namespace common

    class ShaderCompiler : public core::ref_counted_object
    {
      public:
        virtual ~ShaderCompiler() = default;

        static auto create(asset::fx::APIType const apiType) -> core::ref_ptr<ShaderCompiler>;

        virtual auto compileFromFile(std::filesystem::path const& filePath,
                                     std::string& errors) -> std::optional<asset::ShaderFile> = 0;
    };
} // namespace ionengine::shadersys