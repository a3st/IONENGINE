// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "texture.h"

namespace ionengine::rendersystem {

class TexturePool {
public:

    struct Key {

        bool operator<(const Key& rhs) const {
            return false;
        }
    };

    TexturePool(gfx::Device* device) : m_device(device) {

        assert(device && "pointer to device is null");
    }

    Texture* get_texture(const std::string& name) {

        
    }

    void release(Texture* texture) {

    }

private:

    gfx::Device* m_device;

    std::map<std::string, std::unique_ptr<Texture>> m_textures;
};

}