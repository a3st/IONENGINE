// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "engine_environment.hpp"
#include "graphics_pipeline.hpp"
#include "iengine_module.hpp"
#include "platform/platform.hpp"
#include "rhi/rhi.hpp"
#include "shader.hpp"
#include "texture_pool.hpp"

namespace ionengine
{
    class Graphics : public IEngineModule
    {
        struct FrameData
        {
            core::ref_ptr<TexturePool> frameBufferPool;
        };

      public:
        struct ModuleOptions
        {
            std::filesystem::path shadersPath;
            size_t stagingBufferSize;
            core::ref_ptr<GraphicsPipeline> graphicsPipeline;
        };

        Graphics(core::ref_ptr<platform::App> app, EngineEnvironment& environment);

        Graphics(core::ref_ptr<platform::App> app, EngineEnvironment& environment, ModuleOptions const& options);

        auto initialize() -> void override;

        auto shutdown() -> void override
        {
        }

        auto getPriority() const -> uint16_t override
        {
            return std::to_underlying(EngineModulePriority::Core) + 10;
        }

      private:
        EngineEnvironment& _environment;
        core::ref_ptr<platform::App> _app;
        core::ref_ptr<rhi::RHI> _rhi;
        core::ref_ptr<GraphicsPipeline> _curGraphicsPipeline;
        std::filesystem::path _shadersPath;
        std::vector<FrameData> _frames;

        auto onWindowResized(platform::WindowEvent const& event) -> void;

        auto onWindowUpdated() -> void;
    };
} // namespace ionengine