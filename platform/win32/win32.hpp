// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "platform/platform.hpp"
#define NOMINMAX
#define UNICODE
#include <windows.h>

namespace ionengine::platform
{
    class Win32App : public App
    {
      public:
        Win32App(AppContext& context, std::string_view const title);

        ~Win32App();

        auto getWindowHandle() -> void* override;

        auto getInstanceHandle() -> void* override;

        auto run() -> void override;

      private:
        HWND window;
        AppContext* context;

        static auto wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT;
    };
} // namespace ionengine::platform