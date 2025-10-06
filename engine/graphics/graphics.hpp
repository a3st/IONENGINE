// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "graphics_pipeline.hpp"
#include "iengine_module.hpp"
#include "platform/platform.hpp"
#include "rhi/rhi.hpp"

namespace ionengine
{
    class Graphics : public IEngineModule
    {
      public:
        struct ModuleOptions
        {
            std::filesystem::path shadersPath;
            size_t stagingBufferSize;
            core::ref_ptr<GraphicsPipeline> graphicsPipeline;
        };

        Graphics(core::ref_ptr<platform::App> app);

      private:
        core::ref_ptr<platform::App> _app;
        core::ref_ptr<rhi::RHI> _rhi;
    };
} // namespace ionengine