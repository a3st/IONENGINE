// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "texture.h"

namespace ionengine::rendersystem {

template<Texture::Usage U> 
class TexturePool {
public:

    struct Key {

        bool operator<(const Key& rhs) const {
            return false;
        }
    };

    TexturePool(gfx::Device* device, const uint32 size) : m_device(device) {

        assert(device && "pointer to device is null");
    }

private:

    gfx::Device* m_device;
};

}