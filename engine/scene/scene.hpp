// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/serialize.hpp"

namespace ionengine
{
    namespace asset
    {
        namespace scn
        {
            struct ComponentData
            {
                std::string guid;
            };

            struct EntityData
            {
                
                std::vector<ComponentData> components;
            };

            struct SceneFile
            {
            };
        } // namespace scn
    } // namespace asset

    class Scene
    {
      public:
        Scene() = default;

      private:
    };
} // namespace ionengine