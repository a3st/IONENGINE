// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <glfw/glfw3.h>
#include <precompiled.h>

#include <platform/window.hpp>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>

using namespace ionengine;
using namespace ionengine::platform;

namespace ionengine::platform {

///
/// @private
///
class Window_GLFW final : public Window {
 public:
    Window_GLFW() noexcept = default;
    ~Window_GLFW();

    ///
    /// Get window client width
    /// @return Client width
    ///
    inline virtual uint32_t width() const override { return _width; }

    ///
    /// Get window client height
    /// @return Client height
    ///
    inline virtual uint32_t height() const override { return _height; }

    ///
    /// Get window native handle
    /// @return Void pointer
    ///
    inline virtual void* native_handle() const override {
        return reinterpret_cast<void*>(glfwGetWin32Window(_window));
    }

    ///
    /// Set window label
    /// @param label Label string
    ///
    inline virtual void label(std::string_view const label) override {
        glfwSetWindowTitle(_window, label.data());
    }

    ///
    /// Show window cursor
    /// @param show Boolean (true for show, false for hide)
    ///
    inline virtual void cursor(bool const show) override { _cursor = show; }

 private:
    GLFWwindow* _window;
    uint32_t _width;
    uint32_t _height;
    bool _cursor;

    friend class Window;
    friend void poll_events(Window&,
                            std::function<void(WindowEvent const&)> const&);
};

}  // namespace ionengine::platform

Window_GLFW::~Window_GLFW() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

int32_t last_cursor_x;
int32_t last_cursor_y;
bool gamepad_connected;
int32_t gamepad_id;
GLFWgamepadstate gamepad_state;

core::Expected<std::unique_ptr<Window>, std::string> Window::create(
    uint32_t const width, uint32_t const height,
    std::string_view const label) noexcept {
    auto window = std::make_unique<Window_GLFW>();

    auto result = glfwInit();

    if (result != GLFW_TRUE) {
        return core::make_expected<std::unique_ptr<Window>, std::string>(
            "Error during initializing GLFW");
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    window->_window =
        glfwCreateWindow(width, height, label.data(), nullptr, nullptr);

    if (!window->_window) {
        return core::make_expected<std::unique_ptr<Window>, std::string>(
            "Error during initializing window");
    }

    // Initialize class members
    window->_width = width;
    window->_height = height;
    window->_cursor = true;

    glfwGetCursorPos(window->_window,
                     reinterpret_cast<double_t*>(&last_cursor_x),
                     reinterpret_cast<double_t*>(&last_cursor_y));

    gamepad_connected = false;
    gamepad_id = -1;

    // Iterates to all joystick slots and try to find XInput-like gamepad
    for (int32_t jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; ++jid) {
        if (glfwJoystickPresent(jid)) {
            if (glfwJoystickIsGamepad(jid)) {
                gamepad_connected = true;
                gamepad_id = jid;
                std::cout << std::format("Gamepad connected: {0}",
                                         glfwGetGamepadName(jid))
                          << std::endl;
                glfwGetGamepadState(jid, &gamepad_state);
                break;
            }
        }
    }

    // Joystick callback that de/marks active gamepad
    glfwSetJoystickCallback([](int32_t jid, int32_t event) {
        if (event == GLFW_CONNECTED) {
            if (glfwJoystickIsGamepad(jid)) {
                gamepad_connected = true;
                gamepad_id = jid;
                std::cout << std::format("Gamepad connected: {0}",
                                         glfwGetGamepadName(jid))
                          << std::endl;
                glfwGetGamepadState(jid, &gamepad_state);
            }
        } else if (event == GLFW_DISCONNECTED) {
            if (jid == gamepad_id) {
                gamepad_connected = false;
                gamepad_id = -1;
            }
        }
    });

    return core::make_expected<std::unique_ptr<Window>, std::string>(
        std::move(window));
}

std::function<void(WindowEvent const&)> const* cur_loop;

void ionengine::platform::poll_events(
    Window& window, std::function<void(WindowEvent const&)> const& loop) {
    // Pointer to current loop function. Unvailable make it through another way.
    // F*** C callbacks
    cur_loop = &loop;

    while (!glfwWindowShouldClose(static_cast<Window_GLFW&>(window)._window)) {
        // Close window callback that translates into loop function
        glfwSetWindowCloseCallback(
            static_cast<Window_GLFW&>(window)._window, [](GLFWwindow* window) {
                WindowEvent event = {.event_type = WindowEventType::Closed};
                (*cur_loop)(event);
            });

        // Size window callback that translates into loop function
        glfwSetWindowSizeCallback(
            static_cast<Window_GLFW&>(window)._window,
            [](GLFWwindow* window, int32_t width, int32_t height) {
                WindowEvent event = {.event_type = WindowEventType::Sized};
                event.data.window = {static_cast<uint32_t>(width),
                                     static_cast<uint32_t>(height)};
                (*cur_loop)(event);
            });

        // Key window callback that translates into loop function
        glfwSetKeyCallback(
            static_cast<Window_GLFW&>(window)._window,
            [](GLFWwindow* window, int32_t key, int32_t scancode,
               int32_t action, int32_t mods) {
                WindowEvent event = {.event_type =
                                         WindowEventType::KeyboardButton};
                event.data.input = {
                    .key = key,
                    .state = (action == GLFW_PRESS ? InputKeyState::Pressed
                                                   : InputKeyState::Released),
                    .modifier_flags =
                        ([](int32_t const mods) -> InputModiferFlags {
                            InputModiferFlags flags = InputModiferFlags::None;

                            if (mods & GLFW_MOD_CONTROL) {
                                flags = flags | InputModiferFlags::Control;
                            }
                            if (mods & GLFW_MOD_SHIFT) {
                                flags = flags | InputModiferFlags::Shift;
                            }
                            if (mods & GLFW_MOD_ALT) {
                                flags = flags | InputModiferFlags::Alt;
                            }
                            if (mods & GLFW_MOD_CAPS_LOCK) {
                                flags = flags | InputModiferFlags::CapsLock;
                            }
                            return flags;
                        })(mods)};

                (*cur_loop)(event);
            });

        // Cursor position callback that translates into loop function
        glfwSetCursorPosCallback(
            static_cast<Window_GLFW&>(window)._window,
            [](GLFWwindow* window, double_t xpos, double_t ypos) {
                WindowEvent event = {.event_type = WindowEventType::MouseMoved};
                event.data.cursor = {.x = static_cast<int32_t>(xpos),
                                     .y = static_cast<int32_t>(ypos)};

                (*cur_loop)(event);

                event = {.event_type = WindowEventType::MouseAxis};
                event.data.input = {
                    .key = -1,
                    .state = InputKeyState::Unknown,
                    .modifier_flags = InputModiferFlags::None,
                    .x = std::clamp<float_t>(
                        static_cast<float_t>(static_cast<int32_t>(xpos) -
                                             last_cursor_x),
                        -1.0f, 1.0f),
                    .y = std::clamp<float_t>(
                        static_cast<float_t>(static_cast<int32_t>(ypos) -
                                             last_cursor_y),
                        -1.0f, 1.0f)};

                (*cur_loop)(event);

                last_cursor_x = static_cast<int32_t>(xpos);
                last_cursor_y = static_cast<int32_t>(ypos);
            });

        // Cursor scroll callback that translates into loop function
        glfwSetScrollCallback(
            static_cast<Window_GLFW&>(window)._window,
            [](GLFWwindow* window, double_t xoffset, double_t yoffset) {
                WindowEvent event = {.event_type =
                                         WindowEventType::MouseScroll};
                event.data.input = {.key = -1,
                                    .state = InputKeyState::Unknown,
                                    .modifier_flags = InputModiferFlags::None,
                                    .x = static_cast<float_t>(xoffset),
                                    .y = static_cast<float_t>(yoffset)};

                (*cur_loop)(event);
            });

        // Gamepad functionals that translates into loop functions
        if (gamepad_connected) {
            GLFWgamepadstate state;

            if (glfwGetGamepadState(gamepad_id, &state)) {
                for (size_t i = 0; i < 15; ++i) {
                    if (state.buttons[i] && !gamepad_state.buttons[i]) {
                        WindowEvent event = {
                            .event_type = WindowEventType::GamepadButton};
                        event.data.input = {
                            .key = static_cast<int32_t>(i),
                            .state = InputKeyState::Pressed,
                            .modifier_flags = InputModiferFlags::None};

                        (*cur_loop)(event);
                    } else if (!state.buttons[i] && gamepad_state.buttons[i]) {
                        WindowEvent event = {
                            .event_type = WindowEventType::GamepadButton};
                        event.data.input = {
                            .key = static_cast<int32_t>(i),
                            .state = InputKeyState::Released,
                            .modifier_flags = InputModiferFlags::None};

                        (*cur_loop)(event);
                    }
                }

                WindowEvent event = {.event_type =
                                         WindowEventType::GamepadLeftAxis};

                event.data.input = {.key = -1,
                                    .state = InputKeyState::Unknown,
                                    .modifier_flags = InputModiferFlags::None,
                                    .x = state.axes[GLFW_GAMEPAD_AXIS_LEFT_X],
                                    .y = state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y]};

                (*cur_loop)(event);

                event = {.event_type = WindowEventType::GamepadRightAxis};

                event.data.input = {.key = -1,
                                    .state = InputKeyState::Unknown,
                                    .modifier_flags = InputModiferFlags::None,
                                    .x = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_X],
                                    .y = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_Y]};

                (*cur_loop)(event);

                event = {.event_type = WindowEventType::GamepadLeftTrigger};

                event.data.input = {
                    .key = -1,
                    .state = InputKeyState::Unknown,
                    .modifier_flags = InputModiferFlags::None,
                    .y = state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER]};

                (*cur_loop)(event);

                event = {.event_type = WindowEventType::GamepadRightTrigger};

                event.data.input = {
                    .key = -1,
                    .state = InputKeyState::Unknown,
                    .modifier_flags = InputModiferFlags::None,
                    .y = state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER]};

                (*cur_loop)(event);

                gamepad_state = state;
            }
        }

        glfwPollEvents();
    }
}