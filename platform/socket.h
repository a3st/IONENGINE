// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#ifdef IONENGINE_PLATFORM_WIN32
#include "win32/socket_win32.h"
#endif

#ifdef IONENGINE_PLATFORM_ANDROID
#include "android/socket_android.h"
#endif