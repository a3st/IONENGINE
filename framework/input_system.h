// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/singleton.h"
#include "input_types.h"

namespace ionengine {

class InputSystem final : public Singleton<InputSystem> {
DECLARE_SINGLETON(InputSystem)
friend int32 main(int32, char**);
public:

protected:

    InputSystem() {
        
    }

private:

    std::array<bool, 500> m_current_keys;
    std::array<bool, 500> m_previous_keys;

    float m_mouse_x, m_mouse_y;

    void handle_key() {

    }
};

}