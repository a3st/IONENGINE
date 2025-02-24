// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/event.hpp"
#include "core/ref_ptr.hpp"
#include "common.hpp"

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

    class App : public core::ref_counted_object
    {
      public:
        virtual ~App() = default;

        static auto create(std::string_view const title) -> core::ref_ptr<App>;

        virtual auto getWindowHandle() -> void* = 0;

        virtual auto getInstanceHandle() -> void* = 0;

        virtual auto run() -> void = 0;

        virtual auto setEnableMouse(bool const isEnable) -> void = 0;

        core::event<void(InputEvent const&)> inputStateChanged;

        core::event<void(WindowEvent const&)> windowStateChanged;

        core::event<void()> windowUpdated;
    };
} // namespace ionengine::platform