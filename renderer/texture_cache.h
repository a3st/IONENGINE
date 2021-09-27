// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class TextureCache {
public:

    TextureCache(wrapper::Device* device) : m_device(device) {
        
    }

private:

    wrapper::Device* m_device;
};

}