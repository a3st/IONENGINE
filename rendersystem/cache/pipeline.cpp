// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../../precompiled.h"
#include "pipeline.h"

using namespace ionengine::rendersystem;

PipelineCache::PipelineCache(lgfx::Device* device) : device_(device) {

}

lgfx::Pipeline* PipelineCache::GetPipeline(const Key& key) {

    auto it = pipelines_.find(key);
    if(it != pipelines_.end()) {
        return it->second.get();
    } else {
        //auto ret = pipelines_.emplace(key, std::make_unique<lgfx::Pipeline>(device_, ));
        //return ret.first->second.get();
        return nullptr;
    }
}