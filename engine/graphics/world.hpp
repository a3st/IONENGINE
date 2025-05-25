// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "camera.hpp"
#include "entity.hpp"
#include "light.hpp"

namespace ionengine
{
    class World : public core::ref_counted_object
    {
      public:
        World() = default;

        auto addEntity(core::ref_ptr<Entity> const& entity) -> void;

        auto getMeshes() const -> std::unordered_map<size_t, core::ref_ptr<Mesh>> const&;

        auto getCameras() const -> std::unordered_map<size_t, core::ref_ptr<Camera>> const&;

        auto sortMeshes() -> std::vector<size_t>;

      private:
        std::vector<core::ref_ptr<Entity>> entities;

        std::unordered_map<size_t, core::ref_ptr<Camera>> cameras;
        std::unordered_map<size_t, core::ref_ptr<Light>> lights;
        std::unordered_map<size_t, core::ref_ptr<Mesh>> meshes;
    };
} // namespace ionengine