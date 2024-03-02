// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "asset.hpp"
#include "math/color.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class ShaderManager;
    class LinkedDevice;
    class Texture;

    class Material : public Asset
    {
      public:
        Material(LinkedDevice& device, ShaderManager& shader_manager);

        auto load_from_memory(std::span<uint8_t const> const data) -> bool;

        auto create_using_shader(std::string_view const shader_name) -> bool;

        template <typename Type>
        auto set_param(std::string_view const param, Type&& value)
        {
            if constexpr (std::is_same<float, Type>::value || std::is_same<uint32_t, Type>::value)
            {
                set_parameter_data(param, &value, sizeof(Type));
            }
            else if constexpr (std::is_same<math::Color, Type>::value)
            {
                set_parameter_data(param, value.data(), sizeof(Type));
            }
            else if constexpr (std::is_same<Texture, Type>::value)
            {
                set_parameter_texture(param, std::forward<Type>(value));
            }
        }

        auto get_shader() const -> core::ref_ptr<rhi::Shader>;

        auto get_buffer() -> core::ref_ptr<rhi::Buffer>;

      private:
        LinkedDevice* device;
        ShaderManager* shader_manager;
        core::ref_ptr<rhi::Shader> shader;
        core::ref_ptr<rhi::Buffer> buffer;
        std::unordered_map<std::string, std::pair<uint64_t, rhi::shader_file::ElementType>> parameters;
        std::vector<uint8_t> buffer_data;

        auto set_parameter_data(std::string_view const param, void* data, size_t const size) -> void;

        auto set_parameter_texture(std::string_view const param, Texture& texture) -> void;
    };
} // namespace ionengine