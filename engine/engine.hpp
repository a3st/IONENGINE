// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "job_system.hpp"
#include "linked_device.hpp"
#include "model.hpp"
#include "platform/window.hpp"
#include "texture.hpp"

namespace ionengine
{
    class RenderContext;

    using render_func_t = std::function<void(RenderContext&)>;

    class Engine : public core::ref_counted_object
    {
      public:
        Engine(std::vector<std::filesystem::path> const& shader_paths, core::ref_ptr<platform::Window> window);

        auto tick() -> void;

        auto run() -> void;

        auto load_model(std::filesystem::path const& file_path) -> std::tuple<JobFuture, core::ref_ptr<Model>>;

        auto load_texture(std::filesystem::path const& file_path) -> std::tuple<JobFuture, core::ref_ptr<Texture>>;

        render_func_t on_render;

      private:
        core::ref_ptr<platform::Window> window;
        JobSystem job_system;
        LinkedDevice device;
    };
} // namespace ionengine