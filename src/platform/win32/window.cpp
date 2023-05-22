// Copyright © 2020-2023 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "platform/win32/window.hpp"
#include <dwmapi.h>

using namespace ie::platform;

#define THROW_HRESULT_IF_FAILED(HRESULT) if(FAILED(HRESULT))\
{ throw std::runtime_error(std::format("An error occurred while processing the WINAPI (HRESULT: {:04x})", HRESULT)); } 

auto wstring_convert(std::string_view const source) -> std::wstring {
    int32_t length = MultiByteToWideChar(
        CP_UTF8, 
        0, 
        source.data(), 
        static_cast<int32_t>(source.size()),
        nullptr,
        0
    );

    std::wstring dest(length, '\0');

    MultiByteToWideChar(
        CP_UTF8, 
        0, 
        source.data(), 
        static_cast<int32_t>(source.size()),
        dest.data(),
        length
    );
    return dest;
}

auto string_convert(std::wstring_view const source) -> std::string {
    int32_t length = WideCharToMultiByte(
        CP_UTF8, 
        0, 
        source.data(), 
        static_cast<int32_t>(source.size()), 
        nullptr, 
        0, 
        nullptr, 
        nullptr
    );
    
    std::string dest(length, '\0');

    WideCharToMultiByte(
        CP_UTF8, 
        0, 
        source.data(), 
        static_cast<int32_t>(source.size()), 
        dest.data(), 
        static_cast<int32_t>(dest.size()), 
        nullptr, 
        nullptr
    );
    return dest;                                 
}

auto Window::window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT {
    auto window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    switch(msg) {
        case WM_DESTROY: {
            ::PostQuitMessage(0);
        } break;
        case WM_SIZE: {
            
        } break;
        default: {
            return ::DefWindowProc(hwnd, msg, wparam, lparam);
        } break;
    }
    return 0;
}

Window::Window(
    std::string_view const title, 
    uint32_t const width, 
    uint32_t const height
) {

    auto wnd_class = WNDCLASS {
        .lpfnWndProc = window_proc,
        .hInstance = GetModuleHandle(nullptr),
        .lpszClassName = L"IONENGINE"
    };

    if(!::RegisterClass(&wnd_class)) {
        throw std::runtime_error("Failed to register window class");
    }

    window = CreateWindow(
        wnd_class.lpszClassName,
        wstring_convert(title).c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        width,
        height,
        NULL,
        NULL,
        wnd_class.hInstance,
        NULL
    );

    if(!window) {
        throw std::runtime_error("Failed to create window");
    }

    SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    BOOL enabled = TRUE;
    THROW_HRESULT_IF_FAILED(::DwmSetWindowAttribute(window, DWMWA_USE_IMMERSIVE_DARK_MODE, &enabled, sizeof(enabled)));

    ::ShowWindow(window, SW_SHOW);
    ::UpdateWindow(window);
    ::SetFocus(window);
}

auto Window::run() -> void {
    auto msg = MSG {};
    bool running = true;

    while(running) {
        if(GetMessageA(&msg, NULL, 0, 0) != -1) {
            switch(msg.message) {
                case WM_QUIT: {
                    running = false;
                } break;
                case WM_APP: {
                    
                } break;
                default: {
                    if(msg.hwnd) {
                        ::TranslateMessage(&msg);
                        ::DispatchMessageA(&msg);
                    }
                } break;
            }
        }
    }
}

auto Window::get_handle() const -> HWND {
    return window;
}