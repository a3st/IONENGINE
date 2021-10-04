// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "texture.h"

namespace ionengine::rendersystem {

class TextureManager {
public:

    struct Key {

        bool operator<(const Key& rhs) const {
            return false;
        }
    };

    TextureManager(gfx::Device* device) : m_device(device) {

        assert(device && "pointer to device is null");
    }

private:

    gfx::Device* m_device;
};

}