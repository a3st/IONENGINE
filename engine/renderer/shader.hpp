// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine/asset.hpp"
#include "rhi/fx/fxsl.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class LinkedDevice;
    class Renderer;

    class ShaderAsset : public Asset
    {
      public:
        struct ShaderOption
        {
            uint32_t constantIndex;
            rhi::fx::ShaderElementType elementType;
            uint64_t offset;
            size_t size;
        };

        ShaderAsset(LinkedDevice& device);

        auto loadFromFile(std::filesystem::path const& filePath) -> bool;

        //template <typename Type>
        //auto setOptions(std::string_view const option, Type value, Renderer& renderer) -> void;

        //auto getOptions() const -> std::unordered_map<std::string, ShaderOption>;

      private:
        LinkedDevice* device;
        core::ref_ptr<rhi::Shader> shaderProgram;
        std::unordered_map<std::string, ShaderOption> options;
    };
} // namespace ionengine