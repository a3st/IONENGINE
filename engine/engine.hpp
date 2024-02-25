// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "platform/window.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class Engine
    {
      public:
        Engine(std::filesystem::path const shader_path, core::ref_ptr<platform::Window> window);

        auto tick() -> void;

        auto run() -> void;

      private:
        
        core::ref_ptr<platform::Window> window;

        core::ref_ptr<rhi::Device> device;
        core::ref_ptr<rhi::GraphicsContext> graphics_context;
        
    };
} // namespace ionengine