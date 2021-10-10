// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/format.h"

#include "texture.h"

namespace ionengine::rendersystem {

struct TexturePtr {
    Texture* texture;
    uint64 offset;
};

class TexturePool {
public:

    TexturePool(gfx::Device* device, const usize pool_size) : m_device(device), m_pool_size(pool_size), m_offset(0) {

        assert(device && "pointer to device is null");

        m_textures.resize(m_pool_size);
        m_texture_data.resize(m_pool_size);

        std::memset(m_texture_data.data(), 0x0, sizeof(uint8) * m_texture_data.size());

        for(uint64 i = 0; i < m_pool_size; ++i) {
            m_textures[i] = std::make_unique<Texture>(m_device);
        }
    }

    TexturePtr allocate() {

        if(m_offset == m_pool_size) {
            throw std::runtime_error(lib::format<char>("<TexturePool> Allocation error. Memory budget for pool is over"));
        }

        TexturePtr ptr{};

        for(uint64 i = m_offset; i < m_pool_size; ++i) {
            if(m_texture_data[i] == 0x0) {
                ptr.texture = m_textures[i].get();
                ptr.offset = i;

                m_texture_data[i] = 0x1;
                m_offset = i + 1;
                break;
            }
        }
        return ptr;
    }

    void deallocate(const TexturePtr& ptr) {

        m_texture_data[ptr.offset] = 0x0;
        m_offset = ptr.offset;
    }

private:

    gfx::Device* m_device;

    std::vector<std::unique_ptr<Texture>> m_textures;
    uint64 m_offset;
    std::vector<uint8> m_texture_data;

    const usize m_pool_size;
};

}