// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"

namespace ionengine
{
    class LinkedDevice;

    class ShaderManager
    {
      public:
        ShaderManager(LinkedDevice& device);

        auto load_shaders(std::span<std::filesystem::path const> const shaders) -> void;

        auto get_shader_by_name(std::string_view const shader_name) -> core::ref_ptr<rhi::Shader>;

      private:
        LinkedDevice* device;
        std::unordered_map<std::string, core::ref_ptr<rhi::Shader>> shaders;
    };
} // namespace ionengine