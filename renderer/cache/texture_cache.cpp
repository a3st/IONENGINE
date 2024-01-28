// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "texture_cache.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

TextureCache::TextureCache(rhi::Device& device, core::ref_ptr<rhi::MemoryAllocator> allocator) : device(&device), allocator(allocator) {

}

auto TextureCache::get(TextureData const& data, bool const immediate) -> std::optional<core::ref_ptr<rhi::Texture>> {

    auto result = entries.find(data.hash);
    if(result != entries.end()) {
        if(result->second.texture.get_result()) {
            return std::make_optional(result->second.texture.get());
        } else {
            return std::nullopt;
        }
    } else {
        auto resource = TimedResource {
            .texture = device->create_texture(
                allocator,
                data.width,
                data.height,
                data.depth,
                data.mip_levels,
                data.format,
                data.dimension,
                (rhi::TextureUsageFlags)(rhi::TextureUsage::CopyDst | rhi::TextureUsage::ShaderResource),
                data.mip_data
            ),
            .lifetime = 0
        };

        std::optional<core::ref_ptr<rhi::Texture>> ret = std::nullopt;
        if(immediate) {
            ret = std::make_optional(result->second.texture.get());
        }
       
        entries.emplace(data.hash, std::move(resource));
        return ret;
    }
}

auto TextureCache::update(float const dt) -> void {


}