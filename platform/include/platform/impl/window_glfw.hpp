// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <platform/window.hpp>

#include <glfw/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw/glfw3native.h>

namespace ionengine::platform {

///
/// @private
///
class Window_GLFW final : public Window {
 public:
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

 private:
    GLFWwindow* _window;
    uint32_t _width;
    uint32_t _height;
    bool _cursor;

    friend void poll_events(Window&,
                            std::function<void(WindowEvent const&)> const&);
};

}  // namespace ionengine::platform