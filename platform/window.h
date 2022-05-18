// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

enum class WindowEventType {
    Closed,
    Sized,
    Updated,
    KeyboardInput,
    MouseMoved
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
struct WindowEventData<WindowEventType::MouseMoved> {
    int32_t x;
    int32_t y;
};

struct WindowEvent {
    std::variant<
        WindowEventData<WindowEventType::Closed>,
        WindowEventData<WindowEventType::Sized>,
        WindowEventData<WindowEventType::Updated>,
        WindowEventData<WindowEventType::KeyboardInput>,
        WindowEventData<WindowEventType::MouseMoved>
    > data;

    static WindowEvent sized(uint32_t const width, uint32_t const height) {
        return WindowEvent { .data = WindowEventData<WindowEventType::Sized> { .width = width, .height = height } };
    }

    static WindowEvent closed() {
        return WindowEvent { .data = WindowEventData<WindowEventType::Closed> { } };
    }

    static WindowEvent updated() {
        return WindowEvent { .data = WindowEventData<WindowEventType::Updated> { } };
    }

    static WindowEvent keyboard_input(uint32_t const scan_code, InputState const input_state) {
        return WindowEvent { .data = WindowEventData<WindowEventType::KeyboardInput> { .scan_code = scan_code, .input_state = input_state } };
    }

    static WindowEvent mouse_moved(int32_t const x, int32_t const y) {
        return WindowEvent { .data = WindowEventData<WindowEventType::MouseMoved> { .x = x, .y = y } };
    }
};

class Window {
public:

    Window(std::string_view const label, uint32_t const width, uint32_t const height, bool const fullscreen);

    ~Window();

    Window(Window const&) = delete;

    Window(Window&& other) noexcept;

    Window& operator=(Window const&) = delete;

    Window& operator=(Window&& other) noexcept;

    uint32_t client_width() const;

    uint32_t client_height() const;

    void* native_handle() const;

    void label(std::string_view const label);

    std::queue<WindowEvent>& messages();

private:

    struct Impl;
    struct impl_deleter { void operator()(Impl* ptr) const; };
    std::unique_ptr<Impl, impl_deleter> _impl;
};

}
