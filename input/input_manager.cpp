// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <input/input_manager.h>

using namespace ionengine;
using namespace ionengine::input;

InputManager::InputManager() {
    _current_keys.fill(false);
    _previous_keys.fill(false);
    _axis_values.fill(0.0f);
}

void InputManager::update() {
    _previous_keys = _current_keys;
    _axis_values.fill(0.0f);
}

void InputManager::on_keyboard_event(platform::WindowEventData<platform::WindowEventType::KeyboardInput> const& event) {
    _current_keys.at(event.scan_code) = static_cast<bool>(event.input_state);
}

void InputManager::on_mouse_moved_event(platform::WindowEventData<platform::WindowEventType::MouseMoved> const& event) {
    _axis_values.at(static_cast<uint32_t>(AxisCode::MouseX)) = static_cast<float>(event.x_relative);
    _axis_values.at(static_cast<uint32_t>(AxisCode::MouseY)) = static_cast<float>(event.y_relative);
}

bool InputManager::key_down(KeyCode const key_code) const {
    return _current_keys[static_cast<uint32_t>(key_code)] && !_previous_keys[static_cast<uint32_t>(key_code)];
}
    
bool InputManager::key_up(KeyCode const key_code) const {
    return !_current_keys[static_cast<uint32_t>(key_code)] && _previous_keys[static_cast<uint32_t>(key_code)];
}

bool InputManager::key(KeyCode const key_code) const {
    return _current_keys[static_cast<uint32_t>(key_code)];
}

float InputManager::axis(AxisCode const axis_code) const {
    return _axis_values[static_cast<uint32_t>(axis_code)];
}
