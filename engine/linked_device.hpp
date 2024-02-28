#pragma once

#include "core/ref_ptr.hpp"
#include "platform/window.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class LinkedDevice : public core::ref_counted_object
    {
      public:
        LinkedDevice(platform::Window* window);

        auto get_device() -> rhi::Device&
        {
            return *device;
        }

        auto get_graphics_context() -> rhi::GraphicsContext&
        {
            return *graphics_context;
        }

        auto get_copy_context() -> rhi::CopyContext&
        {
            return *copy_context;
        }

        auto get_frame_texture() const -> core::ref_ptr<rhi::Texture>
        {
            return frame_texture;
        }

        auto begin_frame() -> void;

        auto end_frame() -> void;

        auto begin_upload() -> void;

        auto end_upload() -> void;

        auto upload(core::ref_ptr<rhi::Buffer> buffer, std::span<uint8_t const> const data) -> void;

        auto upload(core::ref_ptr<rhi::Texture> texture, std::vector<std::span<uint8_t const>> const& data) -> void;

        auto read(core::ref_ptr<rhi::Texture> texture, std::vector<std::vector<uint8_t>>& data) -> void;

      private:
        std::mutex mutex;
        core::ref_ptr<rhi::Device> device;

        core::ref_ptr<rhi::GraphicsContext> graphics_context;
        core::ref_ptr<rhi::CopyContext> copy_context;

        rhi::Future<rhi::Query> copy_future;
        core::ref_ptr<rhi::Texture> frame_texture;
        bool is_windowed_rendering;
    };
} // namespace ionengine