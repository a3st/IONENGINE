// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "window.hpp"
#include "core/exception.hpp"
#include "precompiled.h"

namespace ionengine::platform
{
    WindowsWindow::WindowsWindow(std::string_view const label, uint32_t const width, uint32_t const height)
        : width(width), height(height)
    {
        auto wnd_class = WNDCLASS{};
        wnd_class.lpszClassName = TEXT("IONENGINE");
        wnd_class.hInstance = GetModuleHandle(nullptr);
        wnd_class.lpfnWndProc = reinterpret_cast<WNDPROC>(window_procedure);
        wnd_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));

        if (!RegisterClass(&wnd_class))
        {
            throw core::Exception("An error occurred while registering the window");
        }

        size_t length = strlen(reinterpret_cast<const char*>(label.data())) + 1;
        size_t result = 0;

        std::u16string out_str(length - 1, 0);

        mbstowcs_s(&result, reinterpret_cast<wchar_t*>(out_str.data()), length,
                   reinterpret_cast<const char*>(label.data()), length - 1);

        window = CreateWindow(wnd_class.lpszClassName, reinterpret_cast<const wchar_t*>(out_str.c_str()),
                              WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
                              0, 0, width, height, nullptr, nullptr, wnd_class.hInstance, nullptr);

        if (!window)
        {
            throw core::Exception("An error occurred while creating the window");
        }

        SetWindowLongPtr(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        ::ShowWindow(window, SW_SHOWDEFAULT);

        raw_devices[0].usUsagePage = 0x01;
        raw_devices[0].usUsage = 0x06;
        raw_devices[0].dwFlags = 0;
        raw_devices[0].hwndTarget = window;

        raw_devices[1].usUsagePage = 0x01;
        raw_devices[1].usUsage = 0x02;
        raw_devices[1].dwFlags = 0;
        raw_devices[1].hwndTarget = window;

        if (!::RegisterRawInputDevices(raw_devices.data(), static_cast<uint32_t>(raw_devices.size()),
                                       sizeof(RAWINPUTDEVICE)))
        {
            throw core::Exception("An error occurred while registering raw input devices");
        }

        show_cursor(true);
    }

    WindowsWindow::~WindowsWindow()
    {
        ::DestroyWindow(window);

        raw_devices[0].usUsagePage = 0x01;
        raw_devices[0].usUsage = 0x06;
        raw_devices[0].dwFlags = RIDEV_REMOVE;
        raw_devices[0].hwndTarget = nullptr;

        raw_devices[1].usUsagePage = 0x01;
        raw_devices[1].usUsage = 0x02;
        raw_devices[1].dwFlags = RIDEV_REMOVE;
        raw_devices[1].hwndTarget = nullptr;

        ::RegisterRawInputDevices(raw_devices.data(), static_cast<uint32_t>(raw_devices.size()),
                                  sizeof(RAWINPUTDEVICE));
    }

    auto WindowsWindow::set_label(std::string_view const label) -> void
    {
        size_t length = strlen(reinterpret_cast<const char*>(label.data())) + 1;
        size_t result = 0;

        std::u16string out_str(length - 1, 0);

        mbstowcs_s(&result, reinterpret_cast<wchar_t*>(out_str.data()), length,
                   reinterpret_cast<const char*>(label.data()), length - 1);

        SetWindowText(window, reinterpret_cast<const wchar_t*>(out_str.c_str()));
    }

    auto WindowsWindow::try_get_event(WindowEvent& event) -> bool
    {
        auto msg = MSG{};
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            DispatchMessage(&msg);
            event = this->event;
            return true;
        }
        else
        {
            return false;
        }
    }

    auto WindowsWindow::window_procedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) -> LRESULT
    {
        auto window_impl = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

        if (!window_impl)
        {
            return DefWindowProc(hwnd, msg, wparam, lparam);
        }

        switch (msg)
        {
            case WM_CLOSE: {
                window_impl->event = WindowEventFactory::Closed();
                break;
            }
            case WM_SIZE: {
                if (wparam & SIZE_MINIMIZED)
                {
                    break;
                }

                WORD width = LOWORD(lparam);
                WORD height = HIWORD(lparam);

                RECT style_rect{};
                ::AdjustWindowRect(
                    &style_rect,
                    WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX, false);

                LONG style_width = style_rect.right - style_rect.left;
                LONG style_height = style_rect.bottom - style_rect.top;

                window_impl->width = std::max<uint32_t>(1, width - style_width);
                window_impl->height = std::max<uint32_t>(1, height - style_height);

                if (wparam & SIZE_MAXIMIZED)
                {
                    window_impl->event = WindowEventFactory::Sized(window_impl->width, window_impl->height);
                }
                break;
            }
            case WM_EXITSIZEMOVE: {
                window_impl->event = WindowEventFactory::Sized(window_impl->width, window_impl->height);
                break;
            }
            case WM_MOUSEMOVE: {
                if (!window_impl->cursor)
                {
                    ::SetCursor(NULL);

                    auto rect = RECT{};
                    if (::GetClientRect(hwnd, &rect))
                    {
                        auto point = POINT{.x = rect.right / 2, .y = rect.bottom / 2};

                        if (::ClientToScreen(hwnd, &point))
                        {
                            ::SetCursorPos(point.x, point.y);
                        }
                    }
                }
                else
                {
                    ::SetCursor(LoadCursor(0, IDC_ARROW));
                }
                break;
            }
            case WM_INPUT: {
                uint8_t buffer[48];
                uint32_t size = 0;

                ::GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &size,
                                  sizeof(RAWINPUTHEADER));
                if (size <= 48)
                {
                    ::GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, buffer, &size,
                                      sizeof(RAWINPUTHEADER));
                }

                RAWINPUT* raw_input = reinterpret_cast<RAWINPUT*>(buffer);
                DWORD device_type = raw_input->header.dwType;

                if (device_type == RIM_TYPEKEYBOARD)
                {
                    auto window_event = WindowEvent{};

                    USHORT key = raw_input->data.keyboard.VKey;
                    USHORT flags = raw_input->data.keyboard.Flags;
                    UINT msg = raw_input->data.keyboard.Message;

                    switch (msg)
                    {
                        case WM_KEYDOWN:
                        case WM_SYSKEYDOWN: {
                            window_event = WindowEventFactory::KeyboardInput(static_cast<uint32_t>(key), InputState::Pressed);
                        }
                        break;
                        case WM_KEYUP:
                        case WM_SYSKEYUP: {
                            window_event = WindowEventFactory::KeyboardInput(static_cast<uint32_t>(key), InputState::Released);
                        }
                        break;
                    }

                    window_impl->event = window_event;
                }
                else if (device_type == RIM_TYPEMOUSE)
                {
                    LONG x = raw_input->data.mouse.lLastX;
                    LONG y = raw_input->data.mouse.lLastY;
                    ULONG buttons = raw_input->data.mouse.ulButtons;
                    USHORT flags = raw_input->data.mouse.usFlags;
                    USHORT button_flags = raw_input->data.mouse.usButtonFlags;
                    USHORT button_data = raw_input->data.mouse.usButtonData;

                    switch (buttons)
                    {
                        case RI_MOUSE_LEFT_BUTTON_DOWN: {
                            window_impl->event = WindowEventFactory::MouseInput(MouseButton::Left, InputState::Pressed);
                            break;
                        }
                        case RI_MOUSE_RIGHT_BUTTON_DOWN: {
                            window_impl->event = WindowEventFactory::MouseInput(MouseButton::Right, InputState::Pressed);
                            break;
                        }
                        case RI_MOUSE_MIDDLE_BUTTON_DOWN: {
                            window_impl->event = WindowEventFactory::MouseInput(MouseButton::Middle, InputState::Pressed);
                            break;
                        }
                        case RI_MOUSE_BUTTON_4_DOWN: {
                            window_impl->event = WindowEventFactory::MouseInput(MouseButton::Four, InputState::Pressed);
                            break;
                        }
                        case RI_MOUSE_BUTTON_5_DOWN: {
                            window_impl->event = WindowEventFactory::MouseInput(MouseButton::Five, InputState::Pressed);
                            break;
                        }
                        case RI_MOUSE_LEFT_BUTTON_UP: {
                            window_impl->event = WindowEventFactory::MouseInput(MouseButton::Left, InputState::Released);
                            break;
                        }
                        case RI_MOUSE_RIGHT_BUTTON_UP: {
                            window_impl->event = WindowEventFactory::MouseInput(MouseButton::Right, InputState::Released);
                            break;
                        }
                        case RI_MOUSE_MIDDLE_BUTTON_UP: {
                            window_impl->event = WindowEventFactory::MouseInput(MouseButton::Middle, InputState::Released);
                            break;
                        }
                        case RI_MOUSE_BUTTON_4_UP: {
                            window_impl->event = WindowEventFactory::MouseInput(MouseButton::Four, InputState::Released);
                            break;
                        }
                        case RI_MOUSE_BUTTON_5_UP: {
                            window_impl->event = WindowEventFactory::MouseInput(MouseButton::Five, InputState::Released);
                            break;
                        }
                    }

                    auto cursor_point = POINT{};
                    if (::GetCursorPos(&cursor_point))
                    {
                        ::ScreenToClient(window_impl->window, &cursor_point);
                    }

                    window_impl->event = WindowEventFactory::MouseMoved(static_cast<int32_t>(cursor_point.x),
                                                                  static_cast<int32_t>(cursor_point.y),
                                                                  static_cast<int32_t>(x), static_cast<int32_t>(y));
                }
                break;
            }
        }
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }
} // namespace ionengine::platform