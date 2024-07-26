// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "platform.hpp"
#include "precompiled.h"

#ifdef IONENGINE_PLATFORM_WIN32
#include "platform/win32/win32.hpp"
#elif IONENGINE_PLATFORM_X11
#include "platform/x11/x11.hpp"
#endif

namespace ionengine::platform
{
    auto App::create(std::string_view const title) -> core::ref_ptr<App>
    {
#ifdef IONENGINE_PLATFORM_WIN32
        return core::make_ref<Win32App>(title);
#elif IONENGINE_PLATFORM_X11
        return core::make_ref<X11App>(title);
#endif
    }
} // namespace ionengine::platform