// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"

namespace ionengine
{
    enum class AssetType
    {
        Model,
        Texture,
        Shader,
        Material
    };

    class Asset : public core::ref_counted_object
    {
    };
} // namespace ionengine