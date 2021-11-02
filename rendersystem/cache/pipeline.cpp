// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../../precompiled.h"
#include "pipeline.h"

using namespace ionengine::rendersystem;

PipelineCache::PipelineCache(lgfx::Device* device) : device_(device) {

}

lgfx::Pipeline* PipelineCache::GetPipeline(
    const lgfx::PipelineType type,
    lgfx::DescriptorLayout* layout,
    const std::span<const lgfx::InputLayoutDesc> inputs,
    const std::span<lgfx::Shader* const> shaders,
    const lgfx::RasterizerDesc& rasterizer,
    const lgfx::DepthStencilDesc& depth_stencil,
    const lgfx::BlendDesc& blend,
    lgfx::RenderPass* render_pass) {

    static PipelineCache::Key kPipelineKey{};
    kPipelineKey.type = type;
    kPipelineKey.layout = layout;
    kPipelineKey.inputs.resize(inputs.size());
    std::copy(inputs.begin(), inputs.end(), kPipelineKey.inputs.begin());
    kPipelineKey.shaders.resize(shaders.size());
    std::copy(shaders.begin(), shaders.end(), kPipelineKey.shaders.begin());
    kPipelineKey.rasterizer = rasterizer;
    kPipelineKey.depth_stencil = depth_stencil;
    kPipelineKey.blend = blend;
    kPipelineKey.render_pass = render_pass;

    auto it = pipelines_.find(kPipelineKey);
    if(it != pipelines_.end()) {
        return it->second.get();
    } else {
        auto ret = pipelines_.emplace(kPipelineKey, std::make_unique<lgfx::Pipeline>(device_, 
            kPipelineKey.type, kPipelineKey.layout, kPipelineKey.inputs, kPipelineKey.shaders, kPipelineKey.rasterizer, kPipelineKey.depth_stencil, kPipelineKey.blend, kPipelineKey.render_pass));
        return ret.first->second.get();
    }
}
