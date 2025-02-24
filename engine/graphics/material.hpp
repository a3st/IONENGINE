// Copyright © 2020-2025 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "shader.hpp"

namespace ionengine
{
    class Material : public core::ref_counted_object
    {
      public:
        Material(core::ref_ptr<Shader> shader);
    };
} // namespace ionengine