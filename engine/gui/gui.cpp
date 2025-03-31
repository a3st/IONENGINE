// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "gui.hpp"
#include "precompiled.h"

namespace ionengine
{
    GUI::GUI(core::ref_ptr<rhi::RHI> RHI)
    {
        instance = this;
    }

    auto GUI::onRender() -> void
    {
    }
} // namespace ionengine