// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "window.hpp"

#ifdef IONENGINE_PLATFORM_WIN32
#include "platform/windows/window.hpp"
#endif

using namespace ionengine::platform;

auto Window::create(std::string_view const label, uint32_t const width, uint32_t const height, bool const fullscreen) -> core::ref_ptr<Window> {

#ifdef IONENGINE_PLATFORM_WIN32
    return core::make_ref<WindowsWindow>(label, width, height, fullscreen);
#endif
}