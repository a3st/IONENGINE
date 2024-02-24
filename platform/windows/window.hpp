// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "platform/window.hpp"
#define NOMINMAX
#define UNICODE
#include <windows.h>

namespace ionengine::platform
{
    class WindowsWindow : public Window
    {
      public:
        WindowsWindow(std::string_view const label, uint32_t const width, uint32_t const height);

        ~WindowsWindow();

        auto get_width() const -> uint32_t override
        {
            return width;
        }

        auto get_height() const -> uint32_t override
        {
            return height;
        }

        auto get_native_handle() const -> void* override
        {
            return reinterpret_cast<void*>(window);
        }

        auto set_label(std::string_view const label) -> void override;

        auto is_cursor() const -> bool override
        {
            return cursor;
        }

        auto show_cursor(bool const show) -> void override
        {
            cursor = show;
        }

        auto try_get_event(WindowEvent& event) -> bool override;

      private:
        HWND window;
        std::array<RAWINPUTDEVICE, 2> raw_devices;
        uint32_t width;
        uint32_t height;
        bool cursor;
        WindowEvent event;

        static auto window_procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT;
    };
} // namespace ionengine::platform