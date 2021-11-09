// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"
#include "descriptor_pool.h"

namespace lgfx {

class Pipeline {

friend class CommandBuffer;

public:

    Pipeline() = default;

    Pipeline(
        Device* const device, 
        const PipelineType type,
        DescriptorLayout* const layout,
        const std::span<const InputLayoutDesc> inputs,
        const std::span<Shader* const> shaders,
        const RasterizerDesc& rasterizer,
        const DepthStencilDesc& depth_stencil,
        const BlendDesc& blend,
        RenderPass* const render_pass);

    Pipeline(const Pipeline&) = delete;

    Pipeline(Pipeline&& rhs) noexcept = default;

    Pipeline& operator=(const Pipeline&) = delete;
    
    Pipeline& operator=(Pipeline&& rhs) noexcept = default;

    inline PipelineType GetType() const { return type_; }
    inline const std::vector<InputLayoutDesc>& GetInputLayoutDescs() const { return inputs_; }
    inline const std::vector<Shader*>& GetShaders() const { return shaders_; }
    inline const RasterizerDesc& GetRasterizerDesc() const { return rasterizer_; }
    inline const DepthStencilDesc& GetDepthStencilDesc() const { return depth_stencil_; }
    inline const BlendDesc& GetBlendDesc() const { return blend_; }

private:

    ComPtr<ID3D12PipelineState> pipeline_state_;

    PipelineType type_;
    std::vector<InputLayoutDesc> inputs_;
    std::vector<Shader*> shaders_;
    RasterizerDesc rasterizer_;
    DepthStencilDesc depth_stencil_;
    BlendDesc blend_;
};

}