// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache.h>
#include <renderer/cache_ptr.h>
#include <renderer/shader_program.h>
#include <renderer/upload_context.h>
#include <asset/technique.h>
#include <lib/sparse_vector.h>

namespace ionengine::renderer {

template<>
class CachePool<ShaderProgram> {
public:

    CachePool(backend::Device& device, UploadContext& upload_context) :
        _device(&device),
        _upload_context(&upload_context) {
        
    }

    CachePtr<ShaderProgram> get(asset::Technique& technique) {
        
        uint64_t const hash = technique.hash();

        if(_data.is_valid(technique.cache_entry())) {
            auto& cache_entry = _data.get(technique.cache_entry());
            
            if(cache_entry.hash != technique.cache_entry()) {


            }

            return cache_entry.value;

        } else {

            auto cache_entry = CacheEntry<CachePtr<ShaderProgram>> {
                .value = CachePtr<ShaderProgram>(ShaderProgram(*_device, technique)),
                .hash = hash
            };

            cache_entry.value.commit_ok();

            technique.cache_entry(_data.push(std::move(cache_entry)));
            return _data.get(technique.cache_entry()).value;
        }
    }

private:

    backend::Device* _device;
    UploadContext* _upload_context;

    lib::SparseVector<CacheEntry<CachePtr<ShaderProgram>>> _data;
};

}
