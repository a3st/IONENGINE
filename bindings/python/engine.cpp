// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "engine/engine.hpp"
#include "core/ref_ptr.hpp"
#include "platform/window.hpp"
#include "precompiled.h"
#include <pybind11/pybind11.h>

PYBIND11_DECLARE_HOLDER_TYPE(Type, ionengine::core::ref_ptr<Type>);

namespace ionengine
{
    namespace py = pybind11;

    PYBIND11_MODULE(engine, self)
    {
        py::class_<platform::Window, core::ref_ptr<platform::Window>>(self, "Window")
            .def(py::init(&platform::Window::create));

        py::class_<Engine>(self, "Engine")
            .def(py::init<std::string, core::ref_ptr<platform::Window>>())
            .def("tick", &Engine::tick)
            .def("run", &Engine::run);
    }
} // namespace ionengine