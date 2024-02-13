// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "asset/asset_manager.hpp"
#include "core/job_system.hpp"
#include "core/ref_ptr.hpp"
#include "platform/window.hpp"
#include "platform/window_loop.hpp"
#include "renderer/renderer.hpp"

namespace ionengine
{
    class Engine
    {
      public:
        Engine(std::string_view const title);

        auto run() -> void;

      private:
        core::ref_ptr<platform::WindowLoop> window_loop;
        core::ref_ptr<platform::Window> window;
        core::ref_ptr<core::JobSystem> job_system;
        core::ref_ptr<renderer::Renderer> renderer;
        core::ref_ptr<renderer::RenderPipeline> render_pipeline;
        core::ref_ptr<AssetManager> asset_manager;
    };
} // namespace ionengine