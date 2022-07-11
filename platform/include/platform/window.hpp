// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <core/include/core/expected.h>

namespace ionengine::platform {

///
/// Window event types
///
enum class WindowEventType {
    Closed,
    Sized,
    Updated,
    MouseMoved,
    KeyboardButton,
    GamepadButton,
    GamepadAxis,
    MouseButton,
    MouseAxis,
    MouseScroll
};

///
/// Input key states
///
enum class InputKeyState {
    Unknown,
    Pressed,
    Released
};

///
/// Input key modifier flags
///
enum class InputModiferFlags : int32_t {
    None,
    Shift = 1 << 0,
    Control = 1 << 1,
    CapsLock = 1 << 2,
    Alt = 1 << 3
};

DECLARE_ENUM_CLASS_BIT_FLAG(InputModiferFlags)

///
/// Window event struct
///
struct WindowEvent {
    WindowEventType event_type;
    union {
        struct {
            uint32_t width;
            uint32_t height;
        } window;
        struct {
            int32_t x;
            int32_t y;
        } cursor;
        struct {
            int32_t key;
            InputKeyState state;
            InputModiferFlags modifier_flags;
            float_t x;
            float_t y;
        } input;
    } data;
};

///
/// Window class
///
class Window {
 public:
    ///
    /// Get window client width
    /// @return Client width
    ///
    virtual uint32_t width() const = 0;

    ///
    /// Get window client height
    /// @return Client height
    ///
    virtual uint32_t height() const = 0;

    ///
    /// Get window native handle
    /// @return Void pointer
    ///
    virtual void* native_handle() const = 0;

    ///
    /// Set window label
    /// @param label Label string
    ///
    virtual void label(std::string_view const label) = 0;

    ///
    /// Show window cursor
    /// @param show Boolean (true for show, false for hide)
    ///
    virtual void cursor(bool const show) = 0;

    ///
    /// Create window
    /// @param width Width window
    /// @param height Height window
    /// @param label Label window
    /// @return Expected window
    ///
    static core::Expected<std::unique_ptr<Window>, std::string> create(
        uint32_t const width, uint32_t const height,
        std::string_view const label) noexcept;
};

///
/// Poll events from window
/// @param loop Loop function
///
void poll_events(Window& window, std::function<void(WindowEvent const&)> const& loop);

}  // namespace ionengine::platform