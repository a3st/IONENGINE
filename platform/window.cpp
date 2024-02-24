// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "window.hpp"
#include "precompiled.h"

#ifdef IONENGINE_PLATFORM_WIN32
#include "platform/windows/window.hpp"
#endif

namespace ionengine::platform
{
    auto Window::create(std::string_view const label, uint32_t const width, uint32_t const height)
        -> core::ref_ptr<Window>
    {
#ifdef IONENGINE_PLATFORM_WIN32
        return core::make_ref<WindowsWindow>(label, width, height);
#endif
    }
} // namespace ionengine::platform