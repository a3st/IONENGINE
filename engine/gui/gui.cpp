// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "gui.hpp"
#include "precompiled.h"

namespace ionengine
{
    GUI::GUI()
    {
        instance = this;
    }

    auto GUI::drawText(std::string_view const text) -> void
    {
    }
} // namespace ionengine