// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "base/types.h"
#include "base/window_event_loop.h"
#include "base/window.h"

#ifdef IONENGINE_PLATFORM_WIN32

#define NOMINMAX
#define UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <xinput.h>
#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

#include "win32/types.h"
#include "win32/window_event_loop.h"
#include "win32/window.h"

#endif

#ifdef IONENGINE_PLATFORM_ANDROID

#include <jni.h>
#include <android/log.h>
#include "platform/android/android_native_app_glue.h"

#endif