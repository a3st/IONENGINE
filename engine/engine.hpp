// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graphics/graphics.hpp"
#include "graphics/upload_manager.hpp"
#include "gui/gui.hpp"
#include "window/window.hpp"

namespace ionengine
{
    class Engine
    {
      public:
        Engine(uint32_t argc, char** argv);

        virtual ~Engine() = default;

        auto run() -> int32_t;

      protected:
        virtual auto onStart() -> void = 0;

        virtual auto onUpdate(float const deltaTime) -> void = 0;

        virtual auto onRender() -> void = 0;

      private:
        core::ref_ptr<platform::App> application;
        core::ref_ptr<rhi::RHI> RHI;

        std::chrono::high_resolution_clock::time_point beginFrameTime;

        std::unique_ptr<Window> window;
        std::unique_ptr<Graphics> graphics;
        std::unique_ptr<GUI> gui;
    };
} // namespace ionengine