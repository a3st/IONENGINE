// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "texture_cache.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

TextureCache::TextureCache(Backend& backend, UploadManager& upload_manager) :
    backend(&backend),
    upload_manager(&upload_manager) 
{

}

auto TextureCache::get(TextureData const& data) -> core::ref_ptr<Texture> {
    
    return nullptr;
}

auto TextureCache::update(float const dt) -> void {

}
