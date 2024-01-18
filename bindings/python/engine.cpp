// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include <pybind11/pybind11.h>
#include "engine/engine.hpp"

using namespace ionengine;
namespace py = pybind11;

PYBIND11_MODULE(engine, self) {
    py::class_<Engine>(self, "Engine")
        .def(py::init<std::string>())
        .def("run", &Engine::run);
}