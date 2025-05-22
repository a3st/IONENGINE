// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "window.hpp"
#include "precompiled.h"

namespace ionengine
{
    Window::Window(core::ref_ptr<platform::App> const& application) : application(application)
    {
        instance = this;
    }

    auto Window::setEnableMouse(bool const isEnable) -> void
    {
        instance->application->setEnableMouse(isEnable);
    }
} // namespace ionengine