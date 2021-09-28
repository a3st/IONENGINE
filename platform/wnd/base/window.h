// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform::wnd {

/**
    @brief Plaftorm Window class

    It's need for management windowing system on OS
*/
class Window {
public:

    virtual ~Window() = default;

    /**
        @brief Get the handle of window
        @return void* pointer to handle
    */
    virtual void* get_handle() const = 0;

    /**
        @brief Get the id of window
        @return uint64 id of window
    */
    virtual uint64 get_id() const = 0;
    
    /**
        @brief Show cursor in window 
        @param show true - show, false - hide cursor
    */
    virtual void show_cursor(const bool show) = 0;

    /**
        @brief Set the label of window
        @param label label of window
    */
    virtual void set_label(const std::string& label) = 0;

    /**
        @brief Set the window size
        @param width width of window
        @param height height of window
    */
    virtual void set_window_size(const uint32 width, const uint32 height) = 0;

    /**
        @brief Get the window size
        @return PhysicalSize PhysicalSize struct that contains size
    */
    virtual PhysicalSize get_window_size() const = 0;

    /**
        @brief Get the client size of window
        @return PhysicalSize PhysicalSize struct that contains size
    */
    virtual PhysicalSize get_client_size() const = 0;
};

}