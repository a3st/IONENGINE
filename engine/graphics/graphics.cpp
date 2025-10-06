// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "graphics.hpp"
#include "precompiled.h"

namespace ionengine
{
    Graphics::Graphics(core::ref_ptr<platform::App> app)
    {
        assert(app && "core::ref_ptr<platform::App> is nullptr");
        _app = app;

        
    }
} // namespace ionengine