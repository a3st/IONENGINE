// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "window.hpp"
#include "precompiled.h"

namespace ionengine
{
    Window::Window(core::ref_ptr<platform::App> application) : application(application)
    {
    }

    auto Window::setEnableMouse(bool const isEnable) -> void
    {
        application->setEnableMouse(isEnable);
    }
} // namespace ionengine