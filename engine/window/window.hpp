// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "iengine_module.hpp"
#include "platform/platform.hpp"

namespace ionengine
{
    class Window : public IEngineModule
    {
      public:
        struct ModuleOptions
        {
        };

        Window(core::ref_ptr<platform::App> app);

        auto setMouseEnabled(bool const enabled) -> void;

      private:
        core::ref_ptr<platform::App> _app;
    };
} // namespace ionengine