// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>
#include <precompiled.h>

#include <platform/include/platform/window.hpp>

using namespace ionengine;
using namespace ionengine::platform;

namespace ionengine::platform {

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

    inline virtual bool has_close() { return glfwWindowShouldClose(_window); }

 private:
    GLFWwindow* _window;
    uint32_t _width;
    uint32_t _height;
    bool _cursor;

    friend class Window;
    friend void poll_events(Window&, std::function<void()> const&);
};

}  // namespace ionengine::platform

Window_GLFW::~Window_GLFW() {
    glfwDestroyWindow(_window);
    glfwTerminate();
}

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

    return core::make_expected<std::unique_ptr<Window>, std::string>(
        std::move(window));
}



void ionengine::platform::poll_events(Window& window, std::function<void()> const& loop) {

    while(!glfwWindowShouldClose(static_cast<Window_GLFW&>(window)._window)) {

        glfwPollEvents();
    }
}