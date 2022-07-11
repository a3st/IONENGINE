// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>
#include <precompiled.h>

#include <platform/include/platform/window.hpp>

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
    virtual void label(std::string_view const label) override {}

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

core::Expected<std::unique_ptr<Window>, std::string> Window::create(
    uint32_t const width, uint32_t const height,
    std::string_view const label) noexcept {
    auto window = std::make_unique<Window_GLFW>();

    auto result = glfwInit();

    if (result != GLFW_TRUE) {
        return core::make_expected<std::unique_ptr<Window>, std::string>(
            "Error during initialize GLFW");
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();

    window->_window =
        glfwCreateWindow(width, height, label.data(), nullptr, nullptr);
    window->_width = width;
    window->_height = height;
    window->_cursor = true;

    glfwGetCursorPos(window->_window,
                     reinterpret_cast<double_t*>(&last_cursor_x),
                     reinterpret_cast<double_t*>(&last_cursor_y));

    glfwSetInputMode(window->_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    return core::make_expected<std::unique_ptr<Window>, std::string>(
        std::move(window));
}

std::function<void(WindowEvent const&)> const* cur_loop;

void ionengine::platform::poll_events(
    Window& window, std::function<void(WindowEvent const&)> const& loop) {
    cur_loop = &loop;

    while (!glfwWindowShouldClose(static_cast<Window_GLFW&>(window)._window)) {
        glfwSetWindowCloseCallback(
            static_cast<Window_GLFW&>(window)._window, [](GLFWwindow* window) {
                WindowEvent event = {.event_type = WindowEventType::Closed};
                (*cur_loop)(event);
            });

        glfwSetWindowSizeCallback(
            static_cast<Window_GLFW&>(window)._window,
            [](GLFWwindow* window, int32_t width, int32_t height) {
                WindowEvent event = {.event_type = WindowEventType::Sized};
                event.data.window = {static_cast<uint32_t>(width),
                                     static_cast<uint32_t>(height)};
                (*cur_loop)(event);
            });

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
                    .y = -std::clamp<float_t>(
                        static_cast<float_t>(static_cast<int32_t>(ypos) -
                                             last_cursor_y),
                        -1.0f, 1.0f)};

                (*cur_loop)(event);

                last_cursor_x = static_cast<int32_t>(xpos);
                last_cursor_y = static_cast<int32_t>(ypos);
            });

        glfwSetJoystickCallback([](int32_t jid, int32_t event) {
            if (event == GLFW_CONNECTED) {
            } else if (event == GLFW_DISCONNECTED) {
            }
        });

        glfwPollEvents();
    }
}