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
        Win32App(std::string_view const title);

        ~Win32App();

        auto getWindowHandle() -> void* override;

        auto getInstanceHandle() -> void* override;

        auto run() -> void override;

      private:
        HWND window;
        std::array<RAWINPUTDEVICE, 2> rawDevices;
        uint32_t width;
        uint32_t height;
        bool cursor;
        WindowEvent event;

        static auto windowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT;
    };
} // namespace ionengine::platform