// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "render_queue.hpp"
#include "precompiled.h"

namespace ionengine
{
    RenderQueue::RenderQueue()
    {
    }

    auto RenderQueue::clear() -> void
    {
        elements.clear();
    }

    auto RenderQueue::push(DrawableData&& drawableData) -> void
    {
        elements.emplace_back(std::move(drawableData));
    }

    RenderQueue::ConstIterator RenderQueue::begin() const
    {
        return elements.begin();
    }

    RenderQueue::ConstIterator RenderQueue::end() const
    {
        return elements.end();
    }

    RenderQueue::Iterator RenderQueue::begin()
    {
        return elements.begin();
    }

    RenderQueue::Iterator RenderQueue::end()
    {
        return elements.end();
    }
} // namespace ionengine