// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <platform/window.h>
#include <input/input_types.h>

namespace ionengine::input {

class InputManager {
public:

    InputManager();

    InputManager(InputManager const&) = delete;

    InputManager(InputManager&&) = delete;

    InputManager& operator=(InputManager const&) = delete;

    InputManager& operator=(InputManager&&) = delete;

    void update();

    void on_keyboard_event(platform::WindowEventData<platform::WindowEventType::KeyboardInput> const& event);

    void on_mouse_moved_event(platform::WindowEventData<platform::WindowEventType::MouseMoved> const& event);

    bool key_down(KeyCode const key_code) const;
    
    bool key_up(KeyCode const key_code) const;

    bool key(KeyCode const key_code) const;

    float axis(AxisCode const axis_code) const;

private:

    std::array<bool, KEY_CODE_COUNT> _current_keys;
    std::array<bool, KEY_CODE_COUNT> _previous_keys;
    std::array<float, AXIS_CODE_COUNT> _axis_values;
};

}
