// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "asset_future.hpp"
#include "core/job_system.hpp"
#include "model.hpp"
#include "texture.hpp"
#include "shader.hpp"

namespace ionengine
{
    class AssetManager : public core::ref_counted_object
    {
      public:
        AssetManager(core::ref_ptr<core::JobSystem> job_system, renderer::Renderer& renderer);

        auto mount(std::filesystem::path const& file_path) -> void;

        auto load_model(std::filesystem::path const& file_path) -> AssetFuture<Model>;

        auto load_texture(std::filesystem::path const& file_path) -> AssetFuture<Texture>;

        auto load_shader(std::filesystem::path const& file_path) -> AssetFuture<Shader>;

      private:
        core::ref_ptr<core::JobSystem> job_system;
        renderer::Renderer* renderer;

        struct RegisteryInfo {
          core::ref_ptr<Asset> asset;
          uint64_t fence_value;
        };

        std::unordered_map<std::filesystem::path, RegisteryInfo> registery;
    };
} // namespace ionengine