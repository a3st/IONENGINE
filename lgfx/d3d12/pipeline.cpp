// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "pipeline.h"
#include "render_pass.h"
#include "descriptor_layout.h"
#include "device.h"
#include "conversion.h"

using namespace lgfx;

Pipeline::Pipeline(Device* device, const PipelineDesc& desc) : desc_(desc) {

    std::vector<D3D12_INPUT_ELEMENT_DESC> input_descs;
    input_descs.resize(desc_.inputs.size());
    for(uint32_t i = 0; i < static_cast<uint32_t>(desc_.inputs.size()); ++i) {
        D3D12_INPUT_ELEMENT_DESC input_desc{};
        input_desc.SemanticName = desc_.inputs[i].semantic.data();
        input_desc.SemanticIndex = desc_.inputs[i].index;
        input_desc.Format = ToDXGIFormat(desc_.inputs[i].format);
        input_desc.InputSlot = desc_.inputs[i].slot;
        input_desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        input_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        input_desc.InstanceDataStepRate = 0;
        input_descs[i] = input_desc;
    }

    D3D12_INPUT_LAYOUT_DESC input_layout_desc{};
    input_layout_desc.pInputElementDescs = input_descs.data();
    input_layout_desc.NumElements = static_cast<uint32_t>(input_descs.size());

    D3D12_RASTERIZER_DESC rasterizer_desc{};
    rasterizer_desc.FillMode = ToD3D12FillMode(desc_.rasterizer.fill_mode);
    rasterizer_desc.CullMode = ToD3D12CullMode(desc_.rasterizer.cull_mode);
    rasterizer_desc.FrontCounterClockwise = false;
    rasterizer_desc.DepthBias = desc_.rasterizer.depth_bias;
    rasterizer_desc.DepthBiasClamp = 0.0f;
    rasterizer_desc.SlopeScaledDepthBias = 0.0f;
    rasterizer_desc.DepthClipEnable = true;
    rasterizer_desc.MultisampleEnable = false;
    rasterizer_desc.AntialiasedLineEnable = false;
    rasterizer_desc.ForcedSampleCount = 0;
    rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    D3D12_DEPTH_STENCILOP_DESC front_face_desc{};
    front_face_desc.StencilFailOp = ToD3D12StencilOp(desc_.depth_stencil.front_face.fail_op);
    front_face_desc.StencilPassOp = ToD3D12StencilOp(desc_.depth_stencil.front_face.pass_op);
    front_face_desc.StencilDepthFailOp = ToD3D12StencilOp(desc_.depth_stencil.front_face.depth_fail_op);
    front_face_desc.StencilFunc = ToD3D12ComparisonFunc(desc_.depth_stencil.front_face.func);

    D3D12_DEPTH_STENCILOP_DESC back_face_desc{};
    back_face_desc.StencilFailOp = ToD3D12StencilOp(desc_.depth_stencil.back_face.fail_op);
    back_face_desc.StencilPassOp = ToD3D12StencilOp(desc_.depth_stencil.back_face.pass_op);
    back_face_desc.StencilDepthFailOp = ToD3D12StencilOp(desc_.depth_stencil.back_face.depth_fail_op);
    back_face_desc.StencilFunc = ToD3D12ComparisonFunc(desc_.depth_stencil.back_face.func);

    D3D12_DEPTH_STENCIL_DESC depth_stencil_desc{};
    depth_stencil_desc.DepthEnable = desc_.depth_stencil.depth_test_enable;
    depth_stencil_desc.DepthWriteMask = desc_.depth_stencil.depth_write_enable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    // depth_stencil_desc.DepthBoundsTestEnable = desc.depth_stencil.depth_bounds_test_enable;
    depth_stencil_desc.DepthFunc = ToD3D12ComparisonFunc(desc_.depth_stencil.depth_func);
    depth_stencil_desc.StencilEnable = desc_.depth_stencil.stencil_enable;
    depth_stencil_desc.StencilReadMask = desc_.depth_stencil.stencil_read_mask;
    depth_stencil_desc.StencilWriteMask = desc_.depth_stencil.stencil_write_mask;
    depth_stencil_desc.FrontFace = front_face_desc;
    depth_stencil_desc.BackFace = back_face_desc;

    const RenderPassDesc& render_pass_desc = desc_.render_pass->GetDesc();

    D3D12_BLEND_DESC blend_desc{};
    for(uint32_t i = 0; i < static_cast<uint32_t>(render_pass_desc.colors.size()); ++i) {
        blend_desc.RenderTarget[i].BlendEnable = desc_.blend.blend_enable;
        blend_desc.RenderTarget[i].BlendOp = ToD3D12BlendOp(desc_.blend.blend_op);
        blend_desc.RenderTarget[i].SrcBlend = ToD3D12Blend(desc_.blend.blend_src);
        blend_desc.RenderTarget[i].DestBlend = ToD3D12Blend(desc_.blend.blend_dest);
        blend_desc.RenderTarget[i].BlendOpAlpha = ToD3D12BlendOp(desc_.blend.blend_op_alpha);
        blend_desc.RenderTarget[i].SrcBlendAlpha = ToD3D12Blend(desc_.blend.blend_src_alpha);
        blend_desc.RenderTarget[i].DestBlendAlpha = ToD3D12Blend(desc_.blend.blend_dest_alpha);
        blend_desc.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    }

    std::vector<DXGI_FORMAT> rtv_formats;
    rtv_formats.resize(render_pass_desc.colors.size());
    for(uint32_t i = 0; i < static_cast<uint32_t>(render_pass_desc.colors.size()); ++i) {
        rtv_formats[i] = ToDXGIFormat(render_pass_desc.colors[i].format);
    }

    DXGI_SAMPLE_DESC sample_desc{};
    sample_desc.Count = render_pass_desc.sample_count;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC graphics_pipeline_desc{};
    graphics_pipeline_desc.pRootSignature = desc_.layout->root_signature_.Get();
    graphics_pipeline_desc.InputLayout = input_layout_desc;
    graphics_pipeline_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    graphics_pipeline_desc.RasterizerState = rasterizer_desc;
    graphics_pipeline_desc.DepthStencilState = depth_stencil_desc;
    graphics_pipeline_desc.BlendState = blend_desc;

    for(uint32_t i = 0; i < static_cast<uint32_t>(desc_.shaders.size()); ++i) {
        D3D12_SHADER_BYTECODE shader_code{};
        shader_code.pShaderBytecode = desc_.shaders[i].shader_code.data();
        shader_code.BytecodeLength = desc_.shaders[i].shader_code.size();
        switch(desc_.shaders[i].type) {
            case ShaderModuleType::kVertex: graphics_pipeline_desc.VS = shader_code; break;
            case ShaderModuleType::kPixel: graphics_pipeline_desc.PS = shader_code; break;
            case ShaderModuleType::kGeometry: graphics_pipeline_desc.GS = shader_code; break;
            case ShaderModuleType::kDomain: graphics_pipeline_desc.DS = shader_code; break;
            case ShaderModuleType::kHull: graphics_pipeline_desc.HS = shader_code; break;
        }
    }

    std::memcpy(graphics_pipeline_desc.RTVFormats, rtv_formats.data(), rtv_formats.size() * sizeof(DXGI_FORMAT));
    graphics_pipeline_desc.NumRenderTargets = static_cast<uint32_t>(rtv_formats.size());
    graphics_pipeline_desc.DSVFormat = ToDXGIFormat(render_pass_desc.depth_stencil.format);
    graphics_pipeline_desc.SampleMask = std::numeric_limits<uint32_t>::max();
    graphics_pipeline_desc.SampleDesc = sample_desc;
        
    THROW_IF_FAILED(device->device_->CreateGraphicsPipelineState(&graphics_pipeline_desc, __uuidof(ID3D12PipelineState), reinterpret_cast<void**>(pipeline_state_.GetAddressOf())));
}