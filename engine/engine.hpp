// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "linked_device.hpp"
#include "model.hpp"
#include "platform/window.hpp"
#include "texture.hpp"

namespace ionengine
{
    class Engine
    {
      public:
        Engine(std::filesystem::path const shader_path, core::ref_ptr<platform::Window> window);

        auto tick() -> void;

        auto run() -> void;

        auto load_model(std::filesystem::path const& file_path) -> AssetFuture<Model>;

        auto load_texture(std::filesystem::path const& file_path) -> AssetFuture<Texture>;

      private:
        core::ref_ptr<platform::Window> window;
        core::ref_ptr<JobSystem> job_system;
        core::ref_ptr<LinkedDevice> device;
    };
} // namespace ionengine