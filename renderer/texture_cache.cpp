// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "texture_cache.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

TextureCache::TextureCache(Context& context) :
    context(&context)
{

}

auto TextureCache::get(TextureData const& data) -> core::ref_ptr<Texture> {
    
    auto entry = entries.find(data.hash);

    if(entry != entries.end()) {
        if(!std::get<0>(entry->second)) {
            auto texture = core::make_ref<Texture>(*context, data);
            entries.emplace(data.hash, std::make_tuple(texture, 60));
        }
        return std::get<0>(entry->second);
    } else {
        auto texture = core::make_ref<Texture>(*context, data);
        entries.emplace(data.hash, std::make_tuple(texture, 60));
        return texture;
    }
}

auto TextureCache::update(float const dt) -> void {

    for(auto& entry : entries) {
        if(std::get<0>(entry.second)) {
            if(std::get<1>(entry.second) > 0) {
                std::get<1>(entry.second) -= dt;
            } else {
                std::get<0>(entry.second) = nullptr;
            }
        }
    }
}