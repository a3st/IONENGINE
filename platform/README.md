
<p><h1 align="center">IONENGINE Platform Module</h1></p>

**IONENGINE** (Image Open Engine) is an Graphic 3D Engine written in C++. It can be used to learn and exploring renderer techniques or write cross-platform C++ applications like games. A basic version of engine only includes **Renderer** module.

<p><h1 align="left">Features</h1></p>

* Cross-platform: Windows, Linux (Android in plan)
* Gamepad support (XInput)

<p><h1 align="left">Hello window</h1></p>

```c++
#include <platform/window.hpp>

auto result = platform::Window::create(800, 600, "Hello window");

if (!result.has_value()) {
    std::cerr << result.error() << std::endl;
    exit(EXIT_FAILURE);
}

auto window = std::move(result.ok());

// Poll events while window active (before Closed event will not received)
platform::poll_events(*window, [](platform::WindowEvent const& event) {
    switch (event.event_type) {
        case platform::WindowEventType::Closed: {
            std::cout << "Window is closed" << std::endl;
        } break;
        case platform::WindowEventType::Sized: {
            std::cout << std::format("Window is resized to (width: {0}, height: {1})", 
                eventdata. window.width, event.data.window.height) << std::endl;
        } break;
        case platform::WindowEventType::GamepadButton: {
            std::cout << std::format("Trigger key on Gamepad: key {0}, state {1}", 
                event.data.input.key, (int32_t)event.data.input.state) << std::endl;
        } break;
    }
}

// Window destroys via std::unique_ptr destructor when it is going out from scope
```