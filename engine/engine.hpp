// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "asset_loader.hpp"
#include "linked_device.hpp"
#include "renderer.hpp"
#include "shader_manager.hpp"

namespace ionengine
{
    namespace platform
    {
        class Window;
    }

    class Engine : public core::ref_counted_object
    {
      public:
        Engine(core::ref_ptr<platform::Window> window);

        auto tick() -> void;

        auto run() -> int32_t;

      protected:
        virtual auto init() -> void;

        virtual auto update(float const dt) -> void;

        virtual auto render() -> void;

      private:
        core::ref_ptr<platform::Window> window;
        LinkedDevice device;

      protected:
        ShaderManager shader_manager;
        AssetLoader asset_loader;
        Renderer renderer;
    };
} // namespace ionengine