// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>

namespace ionengine::renderer::frontend {

class GPUTexture {
public:

    GPUTexture() = default;

private:

    backend::Handle<backend::Texture> _texture;
    backend::Handle<backend::Sampler> _sampler;

};

}
