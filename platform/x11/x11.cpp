// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "x11.hpp"
#include "core/error.hpp"
#include "precompiled.h"

namespace ionengine::platform
{
    std::unordered_map<uint32_t, KeyCode> const keyCodes{
        {24, KeyCode::Q},         {25, KeyCode::W},      {26, KeyCode::E},       {27, KeyCode::R},
        {28, KeyCode::T},         {29, KeyCode::Y},      {30, KeyCode::U},       {31, KeyCode::I},
        {32, KeyCode::O},         {33, KeyCode::P},      {38, KeyCode::A},       {39, KeyCode::S},
        {40, KeyCode::D},         {41, KeyCode::F},      {42, KeyCode::G},       {43, KeyCode::H},
        {44, KeyCode::J},         {45, KeyCode::K},      {46, KeyCode::L},       {52, KeyCode::Z},
        {53, KeyCode::X},         {54, KeyCode::C},      {55, KeyCode::V},       {56, KeyCode::B},
        {57, KeyCode::N},         {58, KeyCode::M},      {23, KeyCode::Tab},     {66, KeyCode::CapsLock},
        {50, KeyCode::ShiftL},    {37, KeyCode::CtrlL},  {64, KeyCode::AltL},    {65, KeyCode::Space},
        {108, KeyCode::AltR},     {105, KeyCode::CtrlR}, {62, KeyCode::ShiftR},  {36, KeyCode::Enter},
        {22, KeyCode::Backspace}, {10, KeyCode::_1},     {11, KeyCode::_2},      {12, KeyCode::_3},
        {13, KeyCode::_4},        {14, KeyCode::_5},     {15, KeyCode::_6},      {16, KeyCode::_7},
        {17, KeyCode::_8},        {18, KeyCode::_9},     {19, KeyCode::_0},      {20, KeyCode::Minus},
        {21, KeyCode::Plus},      {49, KeyCode::Quote},  {9, KeyCode::Escape},   {67, KeyCode::F1},
        {68, KeyCode::F2},        {69, KeyCode::F3},     {70, KeyCode::F4},      {71, KeyCode::F5},
        {72, KeyCode::F6},        {73, KeyCode::F7},     {74, KeyCode::F8},      {75, KeyCode::F9},
        {76, KeyCode::F10},       {95, KeyCode::F11},    {96, KeyCode::F12},     {119, KeyCode::Delete},
        {110, KeyCode::Home},     {115, KeyCode::End},   {112, KeyCode::PageUp}, {117, KeyCode::PageDown}};

    X11App::X11App(AppContext& context, std::string_view const title) : context(&context)
    {
        display = ::XOpenDisplay(nullptr);
        if (!display)
        {
            throw core::runtime_error("An error occurred while open the display");
        }

        window = ::XCreateSimpleWindow(display, RootWindow(display, XDefaultScreen(display)), 0, 0, 800, 600, 0, 0, 0);

        ::XStoreName(display, window, std::string(title).c_str());

        WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", 0);
        ::XSetWMProtocols(display, window, &WM_DELETE_WINDOW, 1);

        ::XSelectInput(display, window, KeyPressMask | KeyReleaseMask | StructureNotifyMask);
    }

    X11App::~X11App()
    {
        ::XDestroyWindow(display, window);
        ::XCloseDisplay(display);
    }

    auto X11App::getWindowHandle() -> void*
    {
        return reinterpret_cast<void*>(window);
    }

    auto X11App::getInstanceHandle() -> void*
    {
        return reinterpret_cast<void*>(display);
    }

    auto X11App::run() -> void
    {
        ::XMapWindow(display, window);

        bool running = true;
        XEvent event;

        while (running)
        {
            while (::XPending(display) > 0)
            {
                ::XNextEvent(display, &event);

                switch (event.type)
                {
                    case ConfigureNotify: {
                        WindowEvent windowEvent{.type = WindowEventType::Resize,
                                                .size = {.width = static_cast<uint32_t>(event.xconfigure.width),
                                                         .height = static_cast<uint32_t>(event.xconfigure.height)}};
                        platformInstance->context->onWindowEvent(windowEvent);
                        break;
                    }
                    case ClientMessage: {
                        if (event.xclient.data.l[0] == WM_DELETE_WINDOW)
                        {
                            WindowEvent windowEvent{.type = WindowEventType::Close};
                            platformInstance->context->onWindowEvent(windowEvent);

                            running = false;
                        }
                        break;
                    }
                    case KeyPress: {
                        auto result = keyCodes.find(event.xkey.keycode);
                        if (result != keyCodes.end())
                        {
                            InputEvent inputEvent{.type = InputDeviceType::Keyboard,
                                                  .state = InputState::Pressed,
                                                  .keyCode = result->second};
                            platformInstance->context->onInputEvent(inputEvent);
                        }
                        break;
                    }
                    case KeyRelease: {
                        auto result = keyCodes.find(event.xkey.keycode);
                        if (result != keyCodes.end())
                        {
                            InputEvent inputEvent{.type = InputDeviceType::Keyboard,
                                                  .state = InputState::Released,
                                                  .keyCode = result->second};
                            platformInstance->context->onInputEvent(inputEvent);
                        }
                        break;
                    }
                }
            }

            platformInstance->context->onIdleEvent();
        }
    }
} // namespace ionengine::platform