// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../platform.hpp"
#define NOMINMAX
#define UNICODE
#include <windows.h>

namespace ionengine::platform
{
    struct HWND_deleter
    {
        void operator()(HWND window)
        {
            if (window)
            {
                ::DestroyWindow(window);
                ::UnregisterClass(L"IONENGINE_Window", ::GetModuleHandle(nullptr));
            }
        }
    };

    using UniqueHWND = std::unique_ptr<std::remove_pointer<HWND>::type, HWND_deleter>;

    class Win32App : public App
    {
      public:
        Win32App(std::string_view const title);

        auto getWindowHandle() -> void* override;

        auto getInstanceHandle() -> void* override;

        auto run() -> void override;

        auto setEnableMouse(bool const isEnable) -> void override;

      private:
        UniqueHWND window;

        static auto wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT;
    };
} // namespace ionengine::platform