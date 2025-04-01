// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#include "render_pass.hpp"
#include "precompiled.h"
#include <xxhash.h>

namespace ionengine
{
    RenderPass::RenderPass(std::string_view const debugName) : debugName(debugName)
    {
    }

    auto RenderPass::initializeRenderPass() -> void
    {
        XXH64_state_t* hasher = ::XXH64_createState();
        ::XXH64_reset(hasher, 0);

        for (auto const& color : colors)
        {
            uintptr_t const texturePtr = (uintptr_t)color.texture.get();
            ::XXH64_update(hasher, &texturePtr, sizeof(uintptr_t));
        }

        if (depthStencil.has_value())
        {
            uintptr_t const texturePtr = (uintptr_t)depthStencil.value().texture.get();
            ::XXH64_update(hasher, &texturePtr, sizeof(uintptr_t));
        }

        for (auto const& input : inputs)
        {
            uintptr_t const texturePtr = (uintptr_t)input.texture.get();
            ::XXH64_update(hasher, &texturePtr, sizeof(uintptr_t));
        }

        ::XXH64_update(hasher, debugName.data(), debugName.size());

        hash = ::XXH64_digest(hasher);
        ::XXH64_freeState(hasher);
    }

    auto RenderPass::getHash() const -> uint64_t
    {
        return hash;
    }

    auto RenderPass::getColors() const -> std::span<rhi::RenderPassColorInfo const> const
    {
        return colors;
    }

    auto RenderPass::getInputs() const -> std::span<RenderPassInputInfo const> const
    {
        return inputs;
    }
} // namespace ionengine