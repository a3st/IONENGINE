// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/linked_device.hpp"
#include "renderer/renderer.hpp"

namespace ionengine
{
    namespace platform
    {
        class Window;
    }

    class ShaderAsset;

    class Engine : public core::ref_counted_object
    {
      public:
        Engine(platform::Window* window);

        auto tick() -> void;

        auto loop() -> void;

        auto run() -> void;

      private:
        platform::Window* window;
        LinkedDevice device;

        std::chrono::high_resolution_clock::time_point lastUpdateTime;

      protected:
        virtual auto init() -> void = 0;

        virtual auto update(float const dt) -> void = 0;

        virtual auto render() -> void = 0;

        auto createShaderAsset() -> core::ref_ptr<ShaderAsset>;

        auto createTextureAsset() -> core::ref_ptr<TextureAsset>;

        Renderer renderer;
    };
} // namespace ionengine