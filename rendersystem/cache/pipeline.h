// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class PipelineCache {

public:

    struct Key {

        lgfx::PipelineType type;
        lgfx::DescriptorLayout* layout;
        std::vector<lgfx::InputLayoutDesc> inputs;
        std::vector<lgfx::Shader*> shaders;
        lgfx::RasterizerDesc rasterizer;
        lgfx::DepthStencilDesc depth_stencil;
        lgfx::BlendDesc blend;
        lgfx::RenderPass* render_pass;

        inline bool operator<(const Key& rhs) const {

            return std::tie(type, layout, inputs, shaders, rasterizer, depth_stencil, blend, render_pass) < 
                std::tie(rhs.type, rhs.layout, rhs.inputs, rhs.shaders, rhs.rasterizer, rhs.depth_stencil, rhs.blend, rhs.render_pass);
        }
    };

    PipelineCache(lgfx::Device* device);

    PipelineCache(const PipelineCache&) = delete;
    PipelineCache(PipelineCache&&) = delete;

    PipelineCache& operator=(const PipelineCache&) = delete;
    PipelineCache& operator=(PipelineCache&&) = delete;

    lgfx::Pipeline* GetPipeline(
        const lgfx::PipelineType type,
        lgfx::DescriptorLayout* layout,
        const std::span<const lgfx::InputLayoutDesc> inputs,
        const std::span<lgfx::Shader* const> shaders,
        const lgfx::RasterizerDesc& rasterizer,
        const lgfx::DepthStencilDesc& depth_stencil,
        const lgfx::BlendDesc& blend,
        lgfx::RenderPass* render_pass);

    inline void Reset() {

        pipelines_.clear();
    }

private:

    lgfx::Device* device_;

    std::map<Key, std::unique_ptr<lgfx::Pipeline>> pipelines_;
};

}