// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "shared.h"

namespace ionengine::platform {

class IWindow {
public:

    virtual ~IWindow() = default;

    virtual void* get_handle() const = 0;

    virtual void set_label(std::string const& label) = 0;

    virtual void set_cursor(bool const show) = 0;
    
    virtual bool get_cursor() const = 0;

    virtual Size get_size() const = 0;
};

}