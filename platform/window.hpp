// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine {

namespace platform {

enum class WindowEventType {
    Closed,
    Sized,
    Updated,
    KeyboardInput,
    MouseInput,
    MouseMoved
};

enum class MouseButton {
	Left,
	Right,
	Middle,
	Four,
	Five
};

enum class InputState {
    Released,
    Pressed
};

template<WindowEventType Type>
struct WindowEventData { };

template<>
struct WindowEventData<WindowEventType::Sized> {
    uint32_t width;
    uint32_t height;
};

template<>
struct WindowEventData<WindowEventType::KeyboardInput> {
    uint32_t scan_code;
    InputState input_state;
};

template<>
struct WindowEventData<WindowEventType::MouseInput> {
    MouseButton button;
    InputState input_state;
};

template<>
struct WindowEventData<WindowEventType::MouseMoved> {
    int32_t x;
    int32_t y;
    int32_t x_relative;
    int32_t y_relative;
};

struct WindowEvent {
    std::variant<
        WindowEventData<WindowEventType::Closed>,
        WindowEventData<WindowEventType::Sized>,
        WindowEventData<WindowEventType::Updated>,
        WindowEventData<WindowEventType::KeyboardInput>,
        WindowEventData<WindowEventType::MouseMoved>,
        WindowEventData<WindowEventType::MouseInput>
    > data;

    static WindowEvent Sized(uint32_t const width, uint32_t const height) {
        return WindowEvent { .data = WindowEventData<WindowEventType::Sized> { .width = width, .height = height } };
    }

    static WindowEvent Closed() {
        return WindowEvent { .data = WindowEventData<WindowEventType::Closed> { } };
    }

    static WindowEvent Updated() {
        return WindowEvent { .data = WindowEventData<WindowEventType::Updated> { } };
    }

    static WindowEvent KeyboardInput(uint32_t const scan_code, InputState const input_state) {
        return WindowEvent { .data = WindowEventData<WindowEventType::KeyboardInput> { .scan_code = scan_code, .input_state = input_state } };
    }

    static WindowEvent MouseInput(MouseButton const button, InputState const input_state) {
        return WindowEvent { .data = WindowEventData<WindowEventType::MouseInput> { .button = button, .input_state = input_state } };
    }

    static WindowEvent MouseMoved(int32_t const x, int32_t const y, int32_t const x_relative, int32_t const y_relative) {
        return WindowEvent { .data = WindowEventData<WindowEventType::MouseMoved> { .x = x, .y = y, .x_relative = x_relative, .y_relative = y_relative } };
    }
};

class Window : public core::ref_counted_object {
public:

    static auto create(std::string_view const label, uint32_t const width, uint32_t const height, bool const fullscreen) -> core::ref_ptr<Window>;

    virtual auto get_width() const -> uint32_t = 0;

    virtual auto get_height() const -> uint32_t = 0;

    virtual auto get_native_handle() const -> void* = 0;

    virtual auto set_label(std::string_view const label) -> void = 0;

    virtual auto is_cursor() const -> bool = 0;

    virtual auto show_cursor(bool const show) -> void = 0;

    virtual auto get_queue_message() -> std::queue<WindowEvent>& = 0;
};

}

}