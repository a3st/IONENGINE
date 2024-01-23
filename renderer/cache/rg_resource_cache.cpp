// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "rg_resource_cache.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

RGResourceCache::RGResourceCache(rhi::Device& device) : device(&device) { 

}
/*
auto RGResourceCache::get_from_entries(
    uint32_t const frame_index,
    rhi::TextureFormat const format, 
    uint32_t const sample_count, 
    uint32_t const width, 
    uint32_t const height
) -> RGResource {

    auto entry = entries.find({ format, sample_count, width, height });

    RGResource resource;

    if(entry != entries.end()) {
        resource = std::move(entry->second.front());
        entry->second.pop();
    } else {
        core::ref_ptr<Texture> texture = core::make_ref<Texture2D>(
            *context,
            width,
            height,
            1,
            format,
            sample_count,
            wgpu::TextureUsage::TextureBinding | wgpu::TextureUsage::RenderAttachment
        );
        resource = RGResource {
            .texture = texture
        };
    }
    
    return resource;
}*/

auto RGResourceCache::get(
    uint32_t const frame_index,
    rhi::TextureFormat const format, 
    uint32_t const sample_count, 
    uint32_t const width, 
    uint32_t const height,
    uint64_t const hash
) -> RGResource {

    auto result = allocations.find(hash);

    if(result != allocations.end()) {
        ResourceAllocation const& allocation = result->second;
        return allocation.region->resources[allocation.offset];
    } else {
        auto entry = entries.find({ frame_index, format, sample_count, width, height });

        if(entry != entries.end()) {

        } else {
            std::vector<RGResource> resources;
            std::vector<uint32_t> free;
            std::vector<uint32_t> lifetimes;

            auto resource = RGResource {
                .texture = nullptr
            };

            resources.emplace_back(resource);
            free.emplace_back(0);
            lifetimes.emplace_back(DEFAULT_LIFETIME_VALUE);

            return resource;
        }
    }
}

auto RGResourceCache::update() -> void {

    /*for(auto& [key, value] : resources) {
        if(!value.has_value()) {
            continue;
        }

        auto entry = value.value();

        if(std::get<1>(entry) == 0) {
            RGResource resource = std::move(std::get<0>(entry));

            wgpu::TextureFormat const format = resource.texture->get_texture().getFormat();
            uint32_t const sample_count = resource.texture->get_texture().getSampleCount();
            uint32_t const width = resource.texture->get_texture().getWidth();
            uint32_t const height = resource.texture->get_texture().getHeight();

            entries.at({ format, sample_count, width, height }).push(std::move(resource));

            value = std::nullopt;
        } else {
            std::get<1>(entry) --;
        }
    }*/
}