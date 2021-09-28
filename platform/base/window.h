// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

class Window {
public:

    virtual ~Window() = default;

    virtual void* get_native_handle() const = 0;

    virtual uint64 get_id() const = 0;
    
    virtual void show_cursor(const bool show) = 0;
    virtual void set_label(const std::string& label) = 0;

    virtual void set_window_size(const uint32 width, const uint32 height) = 0;
    virtual PhysicalSize get_window_size() const = 0;

    virtual PhysicalSize get_client_size() const = 0;
    
};

}