// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine
{
    class GUI
    {
      public:
        GUI();

      private:
        inline static GUI* instance;

      public:
        static auto drawText(std::string_view const text) -> void;
    };
} // namespace ionengine