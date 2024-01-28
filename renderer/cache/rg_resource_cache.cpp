// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "precompiled.h"
#include "rg_resource_cache.hpp"

using namespace ionengine;
using namespace ionengine::renderer;

RGResourceCache::RGResourceCache(rhi::Device& device, core::ref_ptr<rhi::MemoryAllocator> allocator) : device(&device), allocator(allocator) { 

}

auto RGResourceCache::get(
    uint32_t const frame_index,
    rhi::TextureFormat const format, 
    uint32_t const sample_count, 
    uint32_t const width, 
    uint32_t const height,
    rhi::TextureUsageFlags const flags,
    uint64_t const hash
) -> core::ref_ptr<rhi::Texture> 
{
    auto result = allocations.find(hash);

    if(result != allocations.end()) {
        ResourceAllocation& allocation = result->second;
        return allocation.chunk->resources[allocation.offset].first;
    } else {
        auto const key = Entry { frame_index, format, sample_count, width, height, flags };
        auto entry = entries.find(key);

        if(entry != entries.end()) {
            core::ref_ptr<rhi::Texture> texture = nullptr;
            uint32_t offset = 0;

            if(static_cast<uint32_t>(entry->second->resources.size()) < entry->second->offset) {    
                texture = device->create_texture(
                    allocator, 
                    width, 
                    height, 
                    1, 
                    1, 
                    format, 
                    rhi::TextureDimension::_2D, 
                    flags,
                    {}
                ).get();

                auto chunk = entry->second.get();
                offset = chunk->offset;
                chunk->resources.emplace_back(std::make_pair(texture, DEFAULT_LIFETIME_VALUE));
                chunk->free.emplace_back(0);
                chunk->offset++;
            }

            if(!texture) {
                auto chunk = entry->second.get();

                for(size_t i = chunk->offset; i < chunk->resources.size(); ++i) {
                    if(chunk->free[i] == 1) {
                        offset = chunk->offset;
                        texture = chunk->resources[i].first;
                        chunk->resources[i].second = DEFAULT_LIFETIME_VALUE;
                        chunk->offset = i + 1;
                        break;
                    }
                }
            }

            auto allocation = ResourceAllocation {
                .chunk = entry->second.get(),
                .offset = offset
            };
            allocations.emplace(hash, allocation);
            return texture;
        } else {
            std::vector<TimedResource> resources;
            std::vector<uint32_t> free;

            auto texture = device->create_texture(
                allocator, 
                width, 
                height, 
                1, 
                1, 
                format, 
                rhi::TextureDimension::_2D, 
                flags,
                {}
            ).get();

            resources.emplace_back(std::make_pair(texture, DEFAULT_LIFETIME_VALUE));
            free.emplace_back(0);

            auto chunk = std::make_unique<Chunk>(resources, free, 0);

            auto allocation = ResourceAllocation {
                .chunk = chunk.get(),
                .offset = 0
            };
            allocations.emplace(hash, allocation);

            entries.emplace(key, std::move(chunk));
            return texture;
        }
    }
}

auto RGResourceCache::update() -> void {

    for(auto it = allocations.begin(); it != allocations.end(); ) {
        auto chunk = it->second.chunk;
        uint32_t const offset = it->second.offset;

        if(chunk->resources[offset].second <= 0) {
            chunk->resources[offset].second = 0;
            chunk->free[offset] = 1;
            chunk->offset = offset;

            it = allocations.erase(it);
        } else {
            chunk->resources[offset].second--;
            it++;
        }
    }
}