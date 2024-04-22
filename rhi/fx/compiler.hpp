// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "fxsl.hpp"

namespace ionengine::rhi::fx
{
    class FXSLCompiler
    {
      public:
        virtual auto add_include_path(std::filesystem::path const& include_path) -> void = 0;

        virtual auto compile(std::filesystem::path const& file_path) -> std::optional<std::vector<uint8_t>> = 0;
    };
} // namespace ionengine::rhi::fx