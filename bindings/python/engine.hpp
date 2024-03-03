// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core.hpp"
#include "engine/engine.hpp"
#include "platform/window.hpp"
#include <pybind11/pybind11.h>
#include <pybind11/stl/filesystem.h>

namespace ionengine
{
    namespace py = pybind11;

    class PyEngine : public Engine
    {
      public:
        PyEngine(core::ref_ptr<platform::Window> window) : Engine(window)
        {
        }

        using Engine::asset_loader;
        using Engine::create_texture;

      protected:
        auto init() -> void override
        {
            PYBIND11_OVERRIDE_PURE(void, Engine, init);
        }

        auto update(float const dt) -> void override
        {
            PYBIND11_OVERRIDE_PURE(void, Engine, update, dt);
        }

        auto render() -> void override
        {
            PYBIND11_OVERRIDE_PURE(void, Engine, render);
        }
    };

    PYBIND11_MODULE(engine, self)
    {
        py::class_<Model, core::ref_ptr<Model>>(self, "Model");
        py::class_<Texture, core::ref_ptr<Texture>>(self, "Texture");

        py::class_<AssetLoader>(self, "AssetLoader")
            .def("load_model", &AssetLoader::load_model)
            .def("load_texture", &AssetLoader::load_texture)
            .def("load_material", &AssetLoader::load_material);

        py::class_<Engine, core::ref_ptr<Engine>, PyEngine>(self, "Engine")
            .def(py::init<core::ref_ptr<platform::Window>>())
            .def("tick", &Engine::tick)
            .def("run", &Engine::run)
            .def("create_texture", &PyEngine::create_texture)
            .def_property_readonly("asset_loader", [](PyEngine const& instance) { return instance.asset_loader; });
    }
} // namespace ionengine