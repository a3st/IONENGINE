// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/singleton.h"
#include "input_types.h"
#include "platform/window_event_handler.h"

namespace ionengine {

class InputSystem final {
public:

    InputSystem() {
        m_previous_keys.fill(false);
        m_current_keys.fill(false);
        m_current_axis.fill(0.0f);
    }

    void tick() {
        m_previous_keys = m_current_keys;
        m_current_axis.fill(0.0f);
    }

    void on_event_handle(const platform::WindowEventHandler& event) {
        switch(event.event_type) {
            case platform::WindowEvent::KeyboardInput: {
                auto input = std::get<platform::KeyboardInput>(event.event);
                m_current_keys[input.scan_code] = static_cast<bool>(input.state);
                break;
            }
            case platform::WindowEvent::MouseInput: {
                auto input = std::get<platform::MouseInput>(event.event);
                switch(input.button) {
                    case platform::MouseButton::Left: m_current_keys[static_cast<uint32>(KeyCode::MouseLeft)] = static_cast<bool>(input.state); break;
                    case platform::MouseButton::Right: m_current_keys[static_cast<uint32>(KeyCode::MouseRight)] = static_cast<bool>(input.state); break;
                    case platform::MouseButton::Middle: m_current_keys[static_cast<uint32>(KeyCode::MouseMiddle)] = static_cast<bool>(input.state); break;
                    case platform::MouseButton::Four: m_current_keys[static_cast<uint32>(KeyCode::MouseFour)] = static_cast<bool>(input.state); break;
                    case platform::MouseButton::Five: m_current_keys[static_cast<uint32>(KeyCode::MouseFive)] = static_cast<bool>(input.state); break;
                }
                break;
            }
            case platform::WindowEvent::MouseMoved: {
                auto input = std::get<platform::MouseMoved>(event.event);
                if(input.relative) {
                    m_current_axis[static_cast<uint32>(AxisCode::MouseX)] = static_cast<float>(input.position.x);
                    m_current_axis[static_cast<uint32>(AxisCode::MouseY)] = static_cast<float>(input.position.y);
                }
                break;
            }
            default: break;
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

private:

    std::array<bool, key_codes_max> m_current_keys;
    std::array<bool, key_codes_max> m_previous_keys;
    std::array<float, axis_codes_max> m_current_axis;
};

}