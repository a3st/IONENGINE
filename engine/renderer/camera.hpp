// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "texture.hpp"
#include "math/matrix.hpp"

namespace ionengine
{
    class Camera : public core::ref_counted_object
    {
      public:
        Camera();

        math::Matf projectionMatrix;

        

      private:
        core::ref_ptr<Texture> targetTexture;
    };
} // namespace ionengine