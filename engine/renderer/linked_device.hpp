// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

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
        LinkedDevice(platform::Window* window);

        auto getDevice() -> rhi::Device&;

        auto getGraphicsContext() -> rhi::GraphicsContext&;

        auto getCopyContext() -> rhi::CopyContext&;

        auto getBackBuffer() const -> core::ref_ptr<rhi::Texture>;

        auto beginFrame() -> void;

        auto endFrame() -> void;

        auto beginUpload() -> void;

        auto endUpload() -> void;

        auto uploadBuffer(core::ref_ptr<rhi::Buffer> buffer, std::span<uint8_t const> const data) -> void;

        auto uploadTexture(core::ref_ptr<rhi::Texture> texture,
                           std::vector<std::span<uint8_t const>> const& data) -> void;

        auto readTexture(core::ref_ptr<rhi::Texture> texture, std::vector<std::vector<uint8_t>>& data) -> void;

      private:
        core::ref_ptr<rhi::Device> device;
        core::ref_ptr<rhi::GraphicsContext> graphicsContext;
        core::ref_ptr<rhi::CopyContext> copyContext;
        bool isWindowedRendering;
        core::ref_ptr<rhi::Texture> backBuffer;
    };
} // namespace ionengine