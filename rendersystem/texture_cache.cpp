// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "texture_cache.h"

using namespace ionengine::rendersystem;

TextureCache::TextureCache() {

}

TextureCache::TextureCache(lgfx::Device* device) {

}

TextureCache::TextureCache(TextureCache&& rhs) noexcept {

}

TextureCache& TextureCache::operator=(TextureCache&& rhs) noexcept {

    return *this;
}

lgfx::Texture* TextureCache::GetTexture(const Key& key) {

    return nullptr;
}

void TextureCache::Clear() {

}