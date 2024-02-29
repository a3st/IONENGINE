// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "job_system.hpp"
#include "linked_device.hpp"
#include "model.hpp"
#include "platform/window.hpp"
#include "texture.hpp"

namespace ionengine
{
    class Engine : public core::ref_counted_object
    {
      public:
        Engine(std::filesystem::path const shader_path, core::ref_ptr<platform::Window> window);

        auto tick() -> void;

        auto run() -> void;

        auto load_model(std::filesystem::path const& file_path) -> std::tuple<JobFuture, core::ref_ptr<Model>>;

        auto load_texture(std::filesystem::path const& file_path) -> std::tuple<JobFuture, core::ref_ptr<Texture>>;

      private:
        core::ref_ptr<platform::Window> window;
        JobSystem job_system;
        LinkedDevice device;
    };
} // namespace ionengine