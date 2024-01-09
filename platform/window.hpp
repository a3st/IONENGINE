// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

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

    static WindowEvent mouse_input(MouseButton const button, InputState const input_state) {
        return WindowEvent { .data = WindowEventData<WindowEventType::MouseInput> { .button = button, .input_state = input_state } };
    }

    static WindowEvent mouse_moved(int32_t const x, int32_t const y, int32_t const x_relative, int32_t const y_relative) {
        return WindowEvent { .data = WindowEventData<WindowEventType::MouseMoved> { .x = x, .y = y, .x_relative = x_relative, .y_relative = y_relative } };
    }
};

class Window {
public:
    //! Create a new window
    /*! 
        \param label a window title
        \param width a window width
        \param height a window height
        \param fullscreen is fullscreen window
    */
    Window(std::string_view const label, uint32_t const width, uint32_t const height, bool const fullscreen);

    ~Window();

    Window(Window const&) = delete;

    Window(Window&& other) noexcept;

    Window& operator=(Window const&) = delete;

    Window& operator=(Window&& other) noexcept;

    //! Get a window width
    uint32_t get_width() const;

    //! Get a window height
    uint32_t get_height() const;

    //! Get a native handle
    void* get_native_handle() const;

    //! Set a window label
    void set_label(std::string_view const label);

    //! Is check a cursor
    bool is_cursor() const;

    //! Show a cursor
    void show_cursor(bool const show);

    std::queue<WindowEvent>& get_queue_message();

private:

    struct Impl;
    struct impl_deleter { void operator()(Impl* ptr) const; };
    std::unique_ptr<Impl, impl_deleter> impl;
};

}