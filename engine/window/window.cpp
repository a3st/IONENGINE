// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "window.hpp"
#include "precompiled.h"

namespace ionengine
{
    Window::Window(core::ref_ptr<platform::App> app)
    {
        assert(app && "core::ref_ptr<platform::App> is nullptr");
        _app = app;
    }

    auto Window::setMouseEnabled(bool const enabled) -> void
    {
        _app->setMouseEnabled(enabled);
    }
} // namespace ionengine