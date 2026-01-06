// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "window.hpp"
#include "precompiled.h"

namespace ionengine
{
    Window::Window(core::ref_ptr<platform::App> app, EngineEnvironment& environment, ModuleOptions const& options)
        : _environment(environment)
    {
        assert(app && "core::ref_ptr<platform::App> is nullptr");
        _app = app;
    }

    Window::Window(core::ref_ptr<platform::App> app, EngineEnvironment& environment) : Window(app, environment, {})
    {
    }

    auto Window::setMouseEnabled(bool const enabled) -> void
    {
        _app->setMouseEnabled(enabled);
    }
} // namespace ionengine