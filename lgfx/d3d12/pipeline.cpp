// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "pipeline.h"
#include "render_pass.h"
#include "descriptor_layout.h"
#include "device.h"
#include "shader.h"
#include "conversion.h"

using namespace lgfx;

Pipeline::Pipeline(
    Device* device, 
    const PipelineType type,
    DescriptorLayout* layout,
    const std::span<InputLayoutDesc> inputs,
    const std::span<Shader*> shaders,
    const RasterizerDesc& rasterizer,
    const DepthStencilDesc& depth_stencil,
    const BlendDesc& blend,
    RenderPass* render_pass) : 
        type_(type),
        inputs_(inputs.begin(), inputs.end()),
        shaders_(shaders.begin(), shaders.end()),
        rasterizer_(rasterizer),
        depth_stencil_(depth_stencil),
        blend_(blend) {

    std::vector<D3D12_INPUT_ELEMENT_DESC> input_descs;
    input_descs.resize(inputs_.size());

    for(size_t i : std::views::iota(0u, inputs_.size())) {
        D3D12_INPUT_ELEMENT_DESC input_desc{};
        input_desc.SemanticName = inputs_[i].semantic.data();
        input_desc.SemanticIndex = inputs[i].index;
        input_desc.Format = ToDXGIFormat(inputs_[i].format);
        input_desc.InputSlot = inputs_[i].slot;
        input_desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        input_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        input_desc.InstanceDataStepRate = 0;
        input_descs[i] = input_desc;
    }

    D3D12_INPUT_LAYOUT_DESC input_layout_desc{};
    input_layout_desc.pInputElementDescs = input_descs.data();
    input_layout_desc.NumElements = static_cast<uint32_t>(input_descs.size());

    D3D12_RASTERIZER_DESC rasterizer_desc{};
    rasterizer_desc.FillMode = ToD3D12FillMode(rasterizer_.fill_mode);
    rasterizer_desc.CullMode = ToD3D12CullMode(rasterizer_.cull_mode);
    rasterizer_desc.FrontCounterClockwise = false;
    rasterizer_desc.DepthBias = rasterizer_.depth_bias;
    rasterizer_desc.DepthBiasClamp = 0.0f;
    rasterizer_desc.SlopeScaledDepthBias = 0.0f;
    rasterizer_desc.DepthClipEnable = true;
    rasterizer_desc.MultisampleEnable = false;
    rasterizer_desc.AntialiasedLineEnable = false;
    rasterizer_desc.ForcedSampleCount = 0;
    rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    D3D12_DEPTH_STENCILOP_DESC front_face_desc{};
    front_face_desc.StencilFailOp = ToD3D12StencilOp(depth_stencil_.front_face.fail_op);
    front_face_desc.StencilPassOp = ToD3D12StencilOp(depth_stencil_.front_face.pass_op);
    front_face_desc.StencilDepthFailOp = ToD3D12StencilOp(depth_stencil_.front_face.depth_fail_op);
    front_face_desc.StencilFunc = ToD3D12ComparisonFunc(depth_stencil_.front_face.func);

    D3D12_DEPTH_STENCILOP_DESC back_face_desc{};
    back_face_desc.StencilFailOp = ToD3D12StencilOp(depth_stencil_.back_face.fail_op);
    back_face_desc.StencilPassOp = ToD3D12StencilOp(depth_stencil_.back_face.pass_op);
    back_face_desc.StencilDepthFailOp = ToD3D12StencilOp(depth_stencil_.back_face.depth_fail_op);
    back_face_desc.StencilFunc = ToD3D12ComparisonFunc(depth_stencil_.back_face.func);

    D3D12_DEPTH_STENCIL_DESC depth_stencil_desc{};
    depth_stencil_desc.DepthEnable = depth_stencil_.depth_test_enable;
    depth_stencil_desc.DepthWriteMask = depth_stencil_.depth_write_enable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    // depth_stencil_desc.DepthBoundsTestEnable = desc.depth_stencil.depth_bounds_test_enable;
    depth_stencil_desc.DepthFunc = ToD3D12ComparisonFunc(depth_stencil_.depth_func);
    depth_stencil_desc.StencilEnable = depth_stencil_.stencil_enable;
    depth_stencil_desc.StencilReadMask = depth_stencil_.stencil_read_mask;
    depth_stencil_desc.StencilWriteMask = depth_stencil_.stencil_write_mask;
    depth_stencil_desc.FrontFace = front_face_desc;
    depth_stencil_desc.BackFace = back_face_desc;

    D3D12_BLEND_DESC blend_desc{};
    for(size_t i : std::views::iota(0u, render_pass->colors_.size())) {
        blend_desc.RenderTarget[i].BlendEnable = blend_.blend_enable;
        blend_desc.RenderTarget[i].BlendOp = ToD3D12BlendOp(blend_.blend_op);
        blend_desc.RenderTarget[i].SrcBlend = ToD3D12Blend(blend_.blend_src);
        blend_desc.RenderTarget[i].DestBlend = ToD3D12Blend(blend_.blend_dest);
        blend_desc.RenderTarget[i].BlendOpAlpha = ToD3D12BlendOp(blend_.blend_op_alpha);
        blend_desc.RenderTarget[i].SrcBlendAlpha = ToD3D12Blend(blend_.blend_src_alpha);
        blend_desc.RenderTarget[i].DestBlendAlpha = ToD3D12Blend(blend_.blend_dest_alpha);
        blend_desc.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    }

    std::vector<DXGI_FORMAT> rtv_formats;
    rtv_formats.resize(render_pass->colors_.size());

    for(size_t i : std::views::iota(0u, render_pass->colors_.size())) {
        rtv_formats[i] = ToDXGIFormat(render_pass->colors_[i].format);
    }

    DXGI_SAMPLE_DESC sample_desc{};
    sample_desc.Count = render_pass->sample_count_;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphics_pipeline_desc{};
    graphics_pipeline_desc.pRootSignature = layout->root_signature_.Get();
    graphics_pipeline_desc.InputLayout = input_layout_desc;
    graphics_pipeline_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    graphics_pipeline_desc.RasterizerState = rasterizer_desc;
    graphics_pipeline_desc.DepthStencilState = depth_stencil_desc;
    graphics_pipeline_desc.BlendState = blend_desc;

    for(size_t i : std::views::iota(0u, shaders_.size())) {
        switch(shaders_[i]->type_) {
            case ShaderType::kVertex: graphics_pipeline_desc.VS = shaders_[i]->shader_; break;
            case ShaderType::kPixel: graphics_pipeline_desc.PS = shaders_[i]->shader_; break;
            case ShaderType::kGeometry: graphics_pipeline_desc.GS = shaders_[i]->shader_; break;
            case ShaderType::kDomain: graphics_pipeline_desc.DS = shaders_[i]->shader_; break;
            case ShaderType::kHull: graphics_pipeline_desc.HS = shaders_[i]->shader_; break;
        }
    }

    std::memcpy(graphics_pipeline_desc.RTVFormats, rtv_formats.data(), rtv_formats.size() * sizeof(DXGI_FORMAT));
    graphics_pipeline_desc.NumRenderTargets = static_cast<uint32_t>(rtv_formats.size());
    graphics_pipeline_desc.DSVFormat = ToDXGIFormat(render_pass->depth_stencil_.format);
    graphics_pipeline_desc.SampleMask = std::numeric_limits<uint32_t>::max();
    graphics_pipeline_desc.SampleDesc = sample_desc;
        
    THROW_IF_FAILED(device->device_->CreateGraphicsPipelineState(&graphics_pipeline_desc, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(pipeline_state_.GetAddressOf())));
}