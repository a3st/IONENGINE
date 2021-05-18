// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/singleton.h"
#include "input_types.h"
#include "platform/window_event_handler.h"

namespace ionengine {

class InputSystem final : public Singleton<InputSystem> {
DECLARE_SINGLETON(InputSystem)
public:

    void tick() {
        std::copy(m_current_keys.begin(), m_current_keys.end(), m_previous_keys);
    }

    void on_key_update(const platform::WindowEventHandler& event) {
        switch(event.event_type) {
            case platform::WindowEvent::KeyboardInput: {
                auto keyboard_input = std::get<platform::KeyboardInput>(event.event);
                m_current_keys[keyboard_input.scan_code] = static_cast<bool>(keyboard_input.state);
                break;
            }
            case platform::WindowEvent::MouseInput: {
                auto mouse_input = std::get<platform::MouseInput>(event.event);
                switch(mouse_input.button) {
                    case platform::MouseButton::Left: m_current_keys[static_cast<uint32>(KeyCode::MouseLeft)] = static_cast<bool>(mouse_input.state); break;
                    case platform::MouseButton::Right: m_current_keys[static_cast<uint32>(KeyCode::MouseRight)] = static_cast<bool>(mouse_input.state); break;
                    case platform::MouseButton::Middle: m_current_keys[static_cast<uint32>(KeyCode::MouseMiddle)] = static_cast<bool>(mouse_input.state); break;
                    case platform::MouseButton::Four: m_current_keys[static_cast<uint32>(KeyCode::MouseFour)] = static_cast<bool>(mouse_input.state); break;
                    case platform::MouseButton::Five: m_current_keys[static_cast<uint32>(KeyCode::MouseFive)] = static_cast<bool>(mouse_input.state); break;
                }
                break;
            }
            case platform::WindowEvent::MouseMoved: {
                auto input = std::get<platform::MouseMoved>(event.event);
                if(input.relative) {
                    m_axis.mouse_x = input.position.x;
                    m_axis.mouse_y = input.position.y;
                }
            }
        }
    }

    bool get_key_down(const KeyCode key_code) const {
        return m_current_keys[static_cast<uint32>(key_code)] && !m_previous_keys[static_cast<uint32>(key_code)];
    }

    bool get_key_up(const KeyCode key_code) const {
        return !m_current_keys[static_cast<uint32>(key_code)] && m_previous_keys[static_cast<uint32>(key_code)];
    }

    bool get_key(const KeyCode key_code) const {
        return m_current_keys[static_cast<uint32>(key_code)];
    }

protected:

    InputSystem() {
        
    }

private:

    std::array<bool, 500> m_current_keys;
    std::array<bool, 500> m_previous_keys;

    struct {
        float mouse_x;
        float mouse_y;
    } m_axis;

    std::unordered_map<
};

}