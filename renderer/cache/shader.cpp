// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/cache/shader.h>

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::cache;

frontend::ShaderProgram& ShaderCache::get(frontend::Context& context, asset::Technique& technique) {

    if(_shaders.is_valid(technique.cache_entry())) {
        auto& cache_entry = _shaders.get(technique.cache_entry());
        
        if(cache_entry.hash != technique.cache_entry()) {


        }

        return cache_entry.value;

    } else {

        auto cache_entry = CacheEntry<frontend::ShaderProgram> {
            .value = frontend::ShaderProgram(context, technique),
            .hash = 0
        };

        technique.cache_entry(_shaders.push(std::move(cache_entry)));
        return _shaders.get(technique.cache_entry()).value;
    }


    
}
