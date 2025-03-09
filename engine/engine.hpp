// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graphics/graphics.hpp"
#include "graphics/upload_manager.hpp"
#include "window/window.hpp"

namespace ionengine
{
    struct EngineContext
    {
        Window* window;
        Graphics* graphics;
    };

    class Engine
    {
      public:
        Engine(uint32_t argc, char** argv);

        auto run() -> int32_t;

      protected:
        virtual auto onStart() -> void = 0;

        virtual auto onUpdate(float const deltaTime) -> void = 0;

        virtual auto onConfigureRender() -> void = 0;

        virtual auto onRender() -> void = 0;

        virtual auto getContext() const -> EngineContext;

      private:
        core::ref_ptr<platform::App> application;
        core::ref_ptr<rhi::RHI> RHI;

        std::chrono::high_resolution_clock::time_point beginFrameTime;

        core::ref_ptr<internal::UploadManager> uploadManager;

        std::unique_ptr<Window> window;
        std::unique_ptr<Graphics> graphics;
    };
} // namespace ionengine