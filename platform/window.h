// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#ifdef IONENGINE_PLATFORM_WIN32
#include "win32/utils_win32.h"
#endif

#include "window_event_handler.h"
#include "window_event_loop.h"

#ifdef IONENGINE_PLATFORM_WIN32
#include "win32/window_win32.h"
#endif

#ifdef IONENGINE_PLATFORM_ANDROID
#include "android/window_android.h"
#endif