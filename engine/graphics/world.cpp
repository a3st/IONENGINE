// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "world.hpp"
#include "precompiled.h"

namespace ionengine
{
    auto World::addEntity(core::ref_ptr<Entity> const& entity) -> void
    {
        entities.emplace_back(entity);

        size_t const lastIndex = entities.size() - 1;

        if (auto cameraEntity = dynamic_cast<Camera*>(entity.get()))
        {
            cameras[lastIndex] = entity;
        }
        else if (auto meshEntity = dynamic_cast<Mesh*>(entity.get()))
        {
            meshes[lastIndex] = entity;
        }
        else if (auto lightEntity = dynamic_cast<Light*>(entity.get()))
        {
            lights[lastIndex] = entity;
        }
    }

    auto World::getMeshes() const -> std::unordered_map<size_t, core::ref_ptr<Mesh>> const&
    {
        return meshes;
    }

    auto World::getCameras() const -> std::unordered_map<size_t, core::ref_ptr<Camera>> const&
    {
        return cameras;
    }

    auto World::sortMeshes() -> std::vector<size_t>
    {
        auto meshKeys = std::views::keys(meshes);
        std::vector<size_t> sortedIndices{meshKeys.begin(), meshKeys.end()};

        /*std::sort(sortedIndices.begin(), sortedIndices.end(),
                  [this](auto const left, auto const right) -> bool { 
                    meshes[left]->
                    return true; });*/

        return sortedIndices;
    }
} // namespace ionengine