// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "texture_view_cache.h"

using namespace ionengine::rendersystem;

TextureViewCache::TextureViewCache() {

}

TextureViewCache::TextureViewCache(lgfx::Device* device) {

}

TextureViewCache::TextureViewCache(TextureViewCache&& rhs) noexcept {

}

TextureViewCache& TextureViewCache::operator=(TextureViewCache&& rhs) noexcept {

    return *this;
}

lgfx::TextureView* TextureViewCache::GetTextureView(const Key& key) {

    return nullptr;
}

void TextureViewCache::Clear() {

}