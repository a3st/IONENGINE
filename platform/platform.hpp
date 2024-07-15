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

    class App : public core::ref_counted_object
    {
      public:
        ~App() = default;

        static auto create(std::string_view const title) -> core::ref_ptr<App>;

        virtual auto getWindowHandle() -> void* = 0;

        virtual auto getInstanceHandle() -> void* = 0;

        template <typename Func>
        auto setIdleCallback(Func&& function) -> void
        {
            idleCallback = [function]() { function(); };
        }

        template <typename Func>
        auto setInputEventCallback(Func&& function) -> void
        {
            inputEventCallback = [function](InputEvent const& event) { function(event); };
        }

        template <typename Func>
        auto setWindowEventCallback(Func&& function) -> void
        {
            windowEventCallback = [function](WindowEvent const& event) { function(event); };
        }

        virtual auto run() -> void = 0;

      protected:
        std::function<void()> idleCallback;
        std::function<void(InputEvent const&)> inputEventCallback;
        std::function<void(WindowEvent const&)> windowEventCallback;
    };
} // namespace ionengine::platform