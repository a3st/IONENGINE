// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "render_queue.hpp"
#include "precompiled.h"

namespace ionengine
{
    RenderQueue::RenderQueue()
    {
    }

    auto RenderQueue::push(DrawableData&& drawableData) -> void
    {
        rawElements.emplace_back(std::move(drawableData));
    }
} // namespace ionengine