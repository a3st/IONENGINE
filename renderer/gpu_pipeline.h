// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/gpu_resource.h>
#include <lib/expected.h>

namespace ionengine::renderer {

enum class GPUPipelineError { 
    /*
        Someday there will be errors here, but so far it is empty to maintain the general style.
    */
};

struct GPUPipeline {
    backend::Handle<backend::Pipeline> pipeline;
    uint64_t hash;

    static lib::Expected<GPUPipeline, lib::Result<GPUPipelineError>> create();
};

template<>
struct GPUResourceDeleter<GPUPipeline> {
    void operator()(backend::Device& device, GPUPipeline const& pipeline) const {
        device.delete_pipeline(pipeline.pipeline);
    }
};

}
