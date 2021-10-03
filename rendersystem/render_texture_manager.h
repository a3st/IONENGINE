// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "render_texture.h"

namespace ionengine::rendersystem {

class RenderTextureManager {
public:

    struct Key {
        uint64 id;

        bool operator<(const Key& rhs) const { 
            return std::tie(id) < std::tie(rhs.id);
        }
    };

    RenderTextureManager(gfx::Device* device) : m_device(device) {

        assert(device && "pointer to device is null");


    }

    lib::Expected<RenderTexture*, std::string> create_render_texture(const uint64 id) {

        Key key = {
            id
        };

        auto it = m_render_textures.find(key);
        if(it != m_render_textures.end()) {
            return lib::make_expected<RenderTexture*, std::string>("<RenderTextureManager> A texture with the same name already exists");
        }

        auto result = m_render_textures.emplace(key, std::make_unique<RenderTexture>(m_device, id));
        return lib::make_expected<RenderTexture*, std::string>(result.first->second.get());
    }

    RenderTexture* get_render_texture(const uint64 id) {

        Key key = {
            id
        };

        auto it = m_render_textures.find(key);
        if(it != m_render_textures.end()) {
            return it->second.get();
        } else {
            return nullptr;
        }
    }

private:

    gfx::Device* m_device;

    std::map<RenderTextureManager::Key, std::unique_ptr<RenderTexture>> m_render_textures;
};
    
}