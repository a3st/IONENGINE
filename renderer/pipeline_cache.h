// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/cache.h>
#include <renderer/resource_ptr.h>
#include <renderer/gpu_program.h>
#include <renderer/gpu_pipeline.h>
#include <renderer/frame_graph.h>

namespace ionengine::renderer {

class PipelineCache {
public:

    PipelineCache(backend::Device& device);

    PipelineCache(PipelineCache const&) = delete;

    PipelineCache(PipelineCache&& other) noexcept;

    PipelineCache& operator=(PipelineCache const&) = delete;

    PipelineCache& operator=(PipelineCache&& other) noexcept;

    ResourcePtr<GPUPipeline> get(GPUProgram const& program, asset::ShaderDrawParameters const& draw_parameters, RenderPass const& render_pass);

private:

    backend::Device* _device;
    std::shared_mutex _mutex;
    std::unordered_map<uint64_t, CacheEntry<ResourcePtr<GPUPipeline>>> _data;
};

uint64_t gpu_pipeline_calculate_hash(GPUProgram const& program, asset::ShaderDrawParameters const& draw_parameters, RenderPass const& render_pass);

}
