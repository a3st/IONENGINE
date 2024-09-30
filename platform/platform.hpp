// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "input/keycodes.hpp"

namespace ionengine::platform
{
    enum class WindowEventType
    {
        Close,
        Resize
    };

    struct WindowEvent
    {
        WindowEventType eventType;
        union {
            struct
            {
                uint32_t width;
                uint32_t height;
            } size;
            uint32_t reserved;
        };
    };

    enum class InputDeviceType
    {
        Keyboard,
        Mouse,
        Gamepad,
        Touch
    };

    enum class InputState
    {
        Pressed,
        Released
    };

    struct InputEvent
    {
        InputDeviceType deviceType;
        InputState state;
        KeyCode keyCode;
        union {
            struct
            {
                int32_t absX;
                int32_t absY;
                int16_t relX;
                int16_t relY;
            } mouse;
            uint32_t reserved;
        };
    };

    class AppContext
    {
      public:
        virtual ~AppContext() = default;

        virtual auto onIdleEvent() -> void = 0;

        virtual auto onInputEvent(InputEvent const& event) -> void = 0;

        virtual auto onWindowEvent(WindowEvent const& event) -> void = 0;
    };

    class App : public core::ref_counted_object
    {
      public:
        virtual ~App() = default;

        static auto create(AppContext& context, std::string_view const title) -> core::ref_ptr<App>;

        virtual auto getWindowHandle() -> void* = 0;

        virtual auto getInstanceHandle() -> void* = 0;

        virtual auto run() -> void = 0;
    };
} // namespace ionengine::platform