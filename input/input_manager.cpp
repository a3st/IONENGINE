// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <input/input_manager.h>

using namespace ionengine;
using namespace ionengine::input;

InputManager::InputManager() {
    _current_keys.fill(false);
    _previous_keys.fill(false);
}

void InputManager::update() {
    _previous_keys = _current_keys;
}

void InputManager::on_keyboard_event(platform::WindowEventData<platform::WindowEventType::KeyboardInput> const& event) {
    _current_keys.at(event.scan_code) = static_cast<bool>(event.input_state);
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
