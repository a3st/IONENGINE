// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core.hpp"
#include "platform/window.hpp"
#include <pybind11/pybind11.h>

namespace ionengine
{
    namespace py = pybind11;

    PYBIND11_MODULE(platform, self)
    {
        py::class_<platform::Window, core::ref_ptr<platform::Window>>(self, "Window")
            .def(py::init(&platform::Window::create));
    }
} // namespace ionengine