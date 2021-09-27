// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/math/vector2.h"

namespace ionengine::framework {

class AppFramework {

public:

    AppFramework(void* window, const uint32 default_width, const uint32 default_height) {
        
    }

    void* get_window() { return m_window; }

    math::Ivector2 get_window_size() const { return m_window_size; }
    math::Ivector2 get_client_size() const { return m_client_size; }

    void on_window_resize(const uint32 new_width, const uint32 new_height) {
        m_window_size.x = new_width;
        m_window_size.y = new_height;
    }

    void on_client_resize(const uint32 new_width, const uint32 new_height) {
        m_client_size.x = new_width;
        m_client_size.y = new_height;
    }

    void on_input() {

    }

private:

    void* m_window;
    bool m_cursor;

    math::Ivector2 m_window_size;
    math::Ivector2 m_client_size;
};

}