// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <platform/window.h>
#include <input/input_types.h>

namespace ionengine::input {

class InputManager {
public:

    InputManager();

    void update();

    void on_keyboard_event(platform::WindowEventData<platform::WindowEventType::KeyboardInput> const& event);

    bool key_down(KeyCode const key_code) const;
    
    bool key_up(KeyCode const key_code) const;

    bool key(KeyCode const key_code) const;

private:

    std::array<bool, KEY_CODE_COUNT> _current_keys;
    std::array<bool, KEY_CODE_COUNT> _previous_keys;
};

}