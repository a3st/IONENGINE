// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine
{
    class Entity
    {
      public:
        std::string name;
        std::vector<std::string> tags;

        Entity() = default;

      private:
    };
} // namespace ionengine