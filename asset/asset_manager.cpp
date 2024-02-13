// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "asset_manager.hpp"
#include "precompiled.h"

namespace ionengine
{
    AssetManager::AssetManager(core::ref_ptr<core::JobSystem> job_system, renderer::Renderer& renderer)
        : job_system(job_system), renderer(&renderer)
    {
    }

    auto AssetManager::load_texture(std::filesystem::path const& file_path) -> AssetFuture<Texture>
    {
        auto result = registery.find(file_path);

        if (result != registery.end())
        {
            return AssetFuture<Texture>(result->second.asset, job_system, result->second.fence_value);
        }
        else
        {
            auto asset = core::make_ref<Texture>(*renderer);
            uint64_t const fence_value = job_system->submit(
                [&, asset, file_path]() -> void {
                    std::vector<uint8_t> data_bytes;
                    {
                        std::basic_ifstream<uint8_t> ifs(file_path, std::ios::binary);
                        ifs.seekg(0, std::ios::end);
                        size_t const size = ifs.tellg();
                        ifs.seekg(0, std::ios::beg);

                        data_bytes.resize(size);
                        ifs.read(data_bytes.data(), size);
                    }

                    asset->load(data_bytes, TextureFormat::KTX2);
                },
                core::JobQueuePriority::Low);
            std::cout << fence_value << std::endl;
            auto registery_info = RegisteryInfo{.asset = asset, .fence_value = fence_value};
            registery.emplace(file_path, registery_info);
            return AssetFuture<Texture>(registery_info.asset, job_system, registery_info.fence_value);
        }
    }

    auto AssetManager::load_model(std::filesystem::path const& file_path) -> AssetFuture<Model>
    {
        auto result = registery.find(file_path);

        if (result != registery.end())
        {
            return AssetFuture<Model>(result->second.asset, job_system, result->second.fence_value);
        }
        else
        {
            auto asset = core::make_ref<Model>(*renderer);
            uint64_t const fence_value = job_system->submit(
                [&, asset, file_path]() -> void {
                    std::vector<uint8_t> data_bytes;
                    {
                        std::basic_ifstream<uint8_t> ifs(file_path, std::ios::binary);
                        ifs.seekg(0, std::ios::end);
                        size_t const size = ifs.tellg();
                        ifs.seekg(0, std::ios::beg);

                        data_bytes.resize(size);
                        ifs.read(data_bytes.data(), size);
                    }

                    asset->load(data_bytes, ModelFormat::GLB);
                },
                core::JobQueuePriority::Low);
            std::cout << fence_value << std::endl;
            auto registery_info = RegisteryInfo{.asset = asset, .fence_value = fence_value};
            registery.emplace(file_path, registery_info);
            return AssetFuture<Model>(registery_info.asset, job_system, registery_info.fence_value);
        }
    }

    auto AssetManager::load_shader(std::filesystem::path const& file_path) -> AssetFuture<Shader>
    {
        auto result = registery.find(file_path);

        if (result != registery.end())
        {
            return AssetFuture<Shader>(result->second.asset, job_system, result->second.fence_value);
        }
        else
        {
            auto asset = core::make_ref<Shader>(*renderer);
            uint64_t const fence_value = job_system->submit(
                [&, asset, file_path]() -> void {
                    std::vector<uint8_t> data_bytes;
                    {
                        std::basic_ifstream<uint8_t> ifs(file_path, std::ios::binary);
                        ifs.seekg(0, std::ios::end);
                        size_t const size = ifs.tellg();
                        ifs.seekg(0, std::ios::beg);

                        data_bytes.resize(size);
                        ifs.read(data_bytes.data(), size);
                    }

                    asset->load(data_bytes);
                },
                core::JobQueuePriority::Low);
            std::cout << fence_value << std::endl;
            auto registery_info = RegisteryInfo{.asset = asset, .fence_value = fence_value};
            registery.emplace(file_path, registery_info);
            return AssetFuture<Shader>(registery_info.asset, job_system, registery_info.fence_value);
        }
    }
} // namespace ionengine