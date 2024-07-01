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

        auto uploadBufferFromBytes(core::ref_ptr<rhi::Buffer> buffer, std::span<uint8_t const> const dataBytes) -> void;

        auto uploadTextureFromBytes(core::ref_ptr<rhi::Texture> texture,
                                    std::vector<std::span<uint8_t const>> const& dataBytes) -> void;

        auto readTextureToBytes(core::ref_ptr<rhi::Texture> texture,
                                std::vector<std::vector<uint8_t>>& dataBytes) -> void;

      private:
        core::ref_ptr<rhi::Device> device;
        core::ref_ptr<rhi::GraphicsContext> graphicsContext;
        core::ref_ptr<rhi::CopyContext> copyContext;
        bool isWindowedRendering;
        core::ref_ptr<rhi::Texture> backBuffer;
    };
} // namespace ionengine