// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "engine/engine.hpp"
#include "core/ref_ptr.hpp"
#include "platform/window.hpp"
#include "precompiled.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl/filesystem.h>

PYBIND11_DECLARE_HOLDER_TYPE(Type, ionengine::core::ref_ptr<Type>);

namespace ionengine
{
    namespace py = pybind11;

    PYBIND11_MODULE(engine, self)
    {
        py::class_<platform::Window, core::ref_ptr<platform::Window>>(self, "Window")
            .def(py::init(&platform::Window::create));

        py::class_<Model, core::ref_ptr<Model>>(self, "Model");

        py::class_<Texture, core::ref_ptr<Texture>>(self, "Texture");

        py::class_<AssetFuture<Model>>(self, "AssetFutureModel")
            .def("get", &AssetFuture<Model>::get)
            .def("get_result", &AssetFuture<Model>::get_result)
            .def("wait", &AssetFuture<Model>::wait);

        py::class_<AssetFuture<Texture>>(self, "AssetFutureTexture")
            .def("get", &AssetFuture<Texture>::get)
            .def("get_result", &AssetFuture<Texture>::get_result)
            .def("wait", &AssetFuture<Texture>::wait);

        py::class_<Engine>(self, "Engine")
            .def(py::init<std::string, core::ref_ptr<platform::Window>>())
            .def("tick", &Engine::tick)
            .def("run", &Engine::run)
            .def("load_model", &Engine::load_model)
            .def("load_texture", &Engine::load_texture);
    }
} // namespace ionengine