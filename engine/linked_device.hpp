// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    namespace platform
    {
        class Window;
    }

    class LinkedDevice
    {
      public:
        LinkedDevice(std::span<std::filesystem::path const> const shaders, platform::Window* window);

        auto get_device() -> rhi::Device&;

        auto get_graphics_context() -> rhi::GraphicsContext&;

        auto get_copy_context() -> rhi::CopyContext&;

        auto get_back_buffer() const -> core::ref_ptr<rhi::Texture>;

        auto begin_frame() -> void;

        auto end_frame() -> void;

        auto begin_upload() -> void;

        auto end_upload() -> void;

        auto upload(core::ref_ptr<rhi::Buffer> buffer, std::span<uint8_t const> const data) -> void;

        auto upload(core::ref_ptr<rhi::Texture> texture, std::vector<std::span<uint8_t const>> const& data) -> void;

        auto read(core::ref_ptr<rhi::Texture> texture, std::vector<std::vector<uint8_t>>& data) -> void;

        auto get_shader_by_name(std::string_view const shader_name) -> core::ref_ptr<rhi::Shader>;

      private:
        std::mutex mutex;
        core::ref_ptr<rhi::Device> device;
        core::ref_ptr<rhi::GraphicsContext> graphics_context;
        core::ref_ptr<rhi::CopyContext> copy_context;
        bool is_windowed_rendering;
        rhi::Future<rhi::Query> copy_future;
        core::ref_ptr<rhi::Texture> back_buffer;
        std::unordered_map<std::string, core::ref_ptr<rhi::Shader>> shaders;

        auto initialize_shaders(std::span<std::filesystem::path const> const shader_paths) -> void;
    };
} // namespace ionengine