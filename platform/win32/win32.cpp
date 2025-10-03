// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "win32.hpp"
#include "core/string_utils.hpp"
#include "precompiled.h"

namespace ionengine::platform
{
    std::unordered_map<uint32_t, KeyCode> const keyCodes{
        {81, KeyCode::Q},        {87, KeyCode::W},      {69, KeyCode::E},      {82, KeyCode::R},
        {84, KeyCode::T},        {89, KeyCode::Y},      {85, KeyCode::U},      {73, KeyCode::I},
        {73, KeyCode::O},        {80, KeyCode::P},      {65, KeyCode::A},      {83, KeyCode::S},
        {68, KeyCode::D},        {70, KeyCode::F},      {71, KeyCode::G},      {72, KeyCode::H},
        {74, KeyCode::J},        {75, KeyCode::K},      {76, KeyCode::L},      {90, KeyCode::Z},
        {88, KeyCode::X},        {67, KeyCode::C},      {86, KeyCode::V},      {66, KeyCode::B},
        {78, KeyCode::N},        {77, KeyCode::M},      {9, KeyCode::Tab},     {20, KeyCode::CapsLock},
        {160, KeyCode::ShiftL},  {162, KeyCode::CtrlL}, {164, KeyCode::AltL},  {32, KeyCode::Space},
        {164, KeyCode::AltR},    {39, KeyCode::CtrlR},  {38, KeyCode::ShiftR}, {13, KeyCode::Enter},
        {8, KeyCode::Backspace}, {49, KeyCode::_1},     {50, KeyCode::_2},     {51, KeyCode::_3},
        {52, KeyCode::_4},       {53, KeyCode::_5},     {54, KeyCode::_6},     {55, KeyCode::_7},
        {56, KeyCode::_8},       {57, KeyCode::_9},     {48, KeyCode::_0},     {189, KeyCode::Minus},
        {187, KeyCode::Plus},    {192, KeyCode::Quote}, {27, KeyCode::Escape}, {112, KeyCode::F1},
        {113, KeyCode::F2},      {114, KeyCode::F3},    {115, KeyCode::F4},    {116, KeyCode::F5},
        {117, KeyCode::F6},      {118, KeyCode::F7},    {119, KeyCode::F8},    {120, KeyCode::F9},
        {121, KeyCode::F10},     {122, KeyCode::F11},   {123, KeyCode::F12},   {46, KeyCode::Delete},
        {36, KeyCode::Home},     {35, KeyCode::End},    {33, KeyCode::PageUp}, {34, KeyCode::PageDown}};

    auto Win32App::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) -> LRESULT
    {
        auto platformInstance = reinterpret_cast<Win32App*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));
        if (!platformInstance)
        {
            return ::DefWindowProc(hWnd, msg, wParam, lParam);
        }

        switch (msg)
        {
            case WM_CLOSE: {
                ::PostQuitMessage(0);

                WindowEvent const windowEvent{.type = WindowEventType::Close};
                platformInstance->windowStateChanged.invoke(windowEvent);
                break;
            }
            case WM_SIZE: {
                uint32_t const width = LOWORD(lParam);
                uint32_t const height = HIWORD(lParam);

                WindowEvent const windowEvent{
                    .type = WindowEventType::Resize,
                    .size = {.width = static_cast<uint32_t>(width), .height = static_cast<uint32_t>(height)}};
                platformInstance->windowStateChanged.invoke(windowEvent);
                break;
            }
            case WM_KEYDOWN: {
                uint32_t const scanCode = (lParam & 0x00ff0000) >> 16;
                uint32_t const virtualKey = ::MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);

                auto result = keyCodes.find(virtualKey);
                if (result != keyCodes.end())
                {
                    InputEvent const inputEvent{
                        .type = InputDeviceType::Keyboard, .state = InputState::Pressed, .keyCode = result->second};
                    platformInstance->inputStateChanged.invoke(inputEvent);
                }
                break;
            }
            case WM_KEYUP: {
                uint32_t const scanCode = (lParam & 0x00ff0000) >> 16;
                uint32_t const virtualKey = ::MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);

                auto result = keyCodes.find(virtualKey);
                if (result != keyCodes.end())
                {
                    InputEvent const inputEvent{
                        .type = InputDeviceType::Keyboard, .state = InputState::Released, .keyCode = result->second};
                    platformInstance->inputStateChanged.invoke(inputEvent);
                }
                break;
            }
            case WM_SYSKEYDOWN: {
                uint32_t const scanCode = (lParam & 0x00ff0000) >> 16;
                uint32_t const virtualKey = ::MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);

                auto result = keyCodes.find(virtualKey);
                if (result != keyCodes.end())
                {
                    InputEvent const inputEvent{
                        .type = InputDeviceType::Keyboard, .state = InputState::Pressed, .keyCode = result->second};
                    platformInstance->inputStateChanged.invoke(inputEvent);
                }
                break;
            }
            case WM_SYSKEYUP: {
                uint32_t const scanCode = (lParam & 0x00ff0000) >> 16;
                uint32_t const virtualKey = ::MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);

                auto result = keyCodes.find(virtualKey);
                if (result != keyCodes.end())
                {
                    InputEvent const inputEvent{
                        .type = InputDeviceType::Keyboard, .state = InputState::Released, .keyCode = result->second};
                    platformInstance->inputStateChanged.invoke(inputEvent);
                }
                break;
            }
        }
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }

    Win32App::Win32App(std::string_view const title) : _cursorEnabled(false)
    {
        WNDCLASS const wndClass{.lpfnWndProc = reinterpret_cast<WNDPROC>(wndProc),
                                .hInstance = ::GetModuleHandle(nullptr),
                                .hbrBackground = static_cast<HBRUSH>(::GetStockObject(BLACK_BRUSH)),
                                .lpszClassName = L"IONENGINE_Window"};

        if (!::RegisterClass(&wndClass))
        {
            throw std::runtime_error("An error occurred while registering the window");
        }

        uint32_t const windowStyle =
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

        RECT rect{.right = 800, .bottom = 600};
        ::AdjustWindowRect(&rect, windowStyle, false);

        _window = UniqueHWND(::CreateWindow(wndClass.lpszClassName, core::string_utils::to_wstring(title).c_str(),
                                            windowStyle, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left,
                                            rect.bottom - rect.top, nullptr, nullptr, wndClass.hInstance, nullptr));
        if (!_window)
        {
            throw std::runtime_error("An error occurred while creating the window");
        }

        ::SetWindowLongPtr(_window.get(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
    }

    auto Win32App::getWindowHandle() -> void*
    {
        return reinterpret_cast<void*>(_window.get());
    }

    auto Win32App::getInstanceHandle() -> void*
    {
        return ::GetModuleHandle(nullptr);
    }

    auto Win32App::setMouseEnabled(bool const enable) -> void
    {
        ::ShowCursor(enable);
        _cursorEnabled = enable;
    }

    auto Win32App::isMouseEnabled() const -> bool
    {
        return _cursorEnabled;
    }

    auto Win32App::run() -> void
    {
        ::ShowWindow(_window.get(), SW_SHOWDEFAULT);

        MSG msg;
        bool running = true;

        while (running)
        {
            if (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                switch (msg.message)
                {
                    case WM_QUIT: {
                        running = false;
                        break;
                    }
                    default: {
                        if (msg.hwnd)
                        {
                            ::TranslateMessage(&msg);
                            ::DispatchMessage(&msg);
                        }
                        break;
                    }
                }
            }
            else
            {
                windowUpdated.invoke();
            }
        }
    }
} // namespace ionengine::platform