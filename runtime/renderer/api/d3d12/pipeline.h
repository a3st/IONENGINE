// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::api {

class D3DPipeline : public Pipeline {
public:

    D3DPipeline(winrt::com_ptr<ID3D12Device4>& device, const GraphicsPipelineDesc& pipeline_desc) 
        : m_device(device), 
        m_pipeline_type(PipelineType::Graphics), 
        m_d3d12_root_signature(static_cast<D3DDescriptorSetLayout&>(pipeline_desc.layout.get()).get_d3d12_root_signature()) {

        // Input Element Description
        // Description of the input data in the pipeline
        std::vector<D3D12_INPUT_ELEMENT_DESC> input_descs;
        for(auto& input : pipeline_desc.inputs) {
            D3D12_INPUT_ELEMENT_DESC input_desc{};
            input_desc.SemanticName = input.semantic_name.c_str();
            input_desc.SemanticIndex = input.index;
            input_desc.Format = static_cast<DXGI_FORMAT>(input.format);
            input_desc.InputSlot = input.slot;
            input_desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
            input_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            input_desc.InstanceDataStepRate = 0;

            input_descs.emplace_back(input_desc);
        }

        D3D12_INPUT_LAYOUT_DESC input_layout_desc{};
        input_layout_desc.pInputElementDescs = input_descs.data();
        input_layout_desc.NumElements = static_cast<uint32>(input_descs.size());

        // Rasterizer Description
        // Description of pipeline rasterization
        D3D12_RASTERIZER_DESC rasterizer_desc{};
        rasterizer_desc.FillMode = convert_fill_mode(pipeline_desc.rasterizer.fill_mode);
        rasterizer_desc.CullMode = convert_cull_mode(pipeline_desc.rasterizer.cull_mode);
        rasterizer_desc.FrontCounterClockwise = false;
        rasterizer_desc.DepthBias = pipeline_desc.rasterizer.depth_bias;
        rasterizer_desc.DepthBiasClamp = 0.0f;
        rasterizer_desc.SlopeScaledDepthBias = 0.0f;
        rasterizer_desc.DepthClipEnable = true;
        rasterizer_desc.MultisampleEnable = false;
        rasterizer_desc.AntialiasedLineEnable = false;
        rasterizer_desc.ForcedSampleCount = 0;
        rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        // Depth Stencil Description
        // Description of the depth and stencil of the pipeline
        D3D12_DEPTH_STENCILOP_DESC front_face_desc{};
        front_face_desc.StencilFailOp = convert_stencil_op(pipeline_desc.depth_stencil.front_face.fail_op);
        front_face_desc.StencilPassOp = convert_stencil_op(pipeline_desc.depth_stencil.front_face.pass_op);
        front_face_desc.StencilDepthFailOp = convert_stencil_op(pipeline_desc.depth_stencil.front_face.depth_fail_op);
        front_face_desc.StencilFunc = convert_comparison_func(pipeline_desc.depth_stencil.front_face.func);

        D3D12_DEPTH_STENCILOP_DESC back_face_desc{};
        back_face_desc.StencilFailOp = convert_stencil_op(pipeline_desc.depth_stencil.back_face.fail_op);
        back_face_desc.StencilPassOp = convert_stencil_op(pipeline_desc.depth_stencil.back_face.pass_op);
        back_face_desc.StencilDepthFailOp = convert_stencil_op(pipeline_desc.depth_stencil.back_face.depth_fail_op);
        back_face_desc.StencilFunc = convert_comparison_func(pipeline_desc.depth_stencil.back_face.func);

        D3D12_DEPTH_STENCIL_DESC depth_stencil_desc{};
        depth_stencil_desc.DepthEnable = pipeline_desc.depth_stencil.depth_test_enable;
        depth_stencil_desc.DepthWriteMask = pipeline_desc.depth_stencil.depth_write_enable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
        // depth_stencil_desc.DepthBoundsTestEnable = desc.depth_stencil.depth_bounds_test_enable;
        depth_stencil_desc.DepthFunc = convert_comparison_func(pipeline_desc.depth_stencil.depth_func);
        depth_stencil_desc.StencilEnable = pipeline_desc.depth_stencil.stencil_enable;
        depth_stencil_desc.StencilReadMask = pipeline_desc.depth_stencil.stencil_read_mask;
        depth_stencil_desc.StencilWriteMask = pipeline_desc.depth_stencil.stencil_write_mask;
        depth_stencil_desc.FrontFace = front_face_desc;
        depth_stencil_desc.BackFace = back_face_desc;

        // Get Render Pass Description
        auto& render_pass_desc = pipeline_desc.render_pass.get().get_desc();

        // Blend Description
        // Description of pipeline blend
        D3D12_BLEND_DESC blend_desc{};
        for(uint32 i = 0; i < render_pass_desc.colors.size(); ++i) {
            if(render_pass_desc.colors[i].format == Format::Unknown) {
                continue;
            }
            auto& render_target = blend_desc.RenderTarget[i];

            render_target.BlendEnable = pipeline_desc.blend.blend_enable;
            render_target.BlendOp = convert_blend_op(pipeline_desc.blend.blend_op);
            render_target.SrcBlend = convert_blend(pipeline_desc.blend.blend_src);
            render_target.DestBlend = convert_blend(pipeline_desc.blend.blend_dest);
            render_target.BlendOpAlpha = convert_blend_op(pipeline_desc.blend.blend_op_alpha);
            render_target.SrcBlendAlpha = convert_blend(pipeline_desc.blend.blend_src_alpha);
            render_target.DestBlendAlpha = convert_blend(pipeline_desc.blend.blend_dest_alpha);
            render_target.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        }

        // Render Target Formats
        std::vector<DXGI_FORMAT> rtv_formats;
        rtv_formats.resize(render_pass_desc.colors.size());
        for(uint32 i = 0; i < render_pass_desc.colors.size(); ++i) {
            if(render_pass_desc.colors[i].format == Format::Unknown) {
                continue;
            }
            rtv_formats[i] = static_cast<DXGI_FORMAT>(render_pass_desc.colors[i].format);
        }

        // Sample Description
        DXGI_SAMPLE_DESC sample_desc{};
        sample_desc.Count = render_pass_desc.sample_count;

        // Graphics Pipeline State Description
        // Description of the pipeline
        D3D12_GRAPHICS_PIPELINE_STATE_DESC graphics_pipeline_desc{};
        graphics_pipeline_desc.pRootSignature = m_d3d12_root_signature.get().get();
        graphics_pipeline_desc.InputLayout = input_layout_desc;
        graphics_pipeline_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        graphics_pipeline_desc.RasterizerState = rasterizer_desc;
        graphics_pipeline_desc.DepthStencilState = depth_stencil_desc;
        graphics_pipeline_desc.BlendState = blend_desc;
        for(auto& shader : pipeline_desc.shaders) {
            switch(shader.get().get_type()) {
                case ShaderType::Vertex: graphics_pipeline_desc.VS = static_cast<D3DShader&>(shader.get()).get_d3d12_shader(); break;
                case ShaderType::Pixel: graphics_pipeline_desc.PS = static_cast<D3DShader&>(shader.get()).get_d3d12_shader(); break;
                case ShaderType::Geometry: graphics_pipeline_desc.GS = static_cast<D3DShader&>(shader.get()).get_d3d12_shader(); break;
                case ShaderType::Domain: graphics_pipeline_desc.DS = static_cast<D3DShader&>(shader.get()).get_d3d12_shader(); break;
                case ShaderType::Hull: graphics_pipeline_desc.HS = static_cast<D3DShader&>(shader.get()).get_d3d12_shader(); break;
            }
        }
        std::memcpy(graphics_pipeline_desc.RTVFormats, rtv_formats.data(), rtv_formats.size() * sizeof(DXGI_FORMAT));
        graphics_pipeline_desc.NumRenderTargets = static_cast<uint32>(rtv_formats.size());
        graphics_pipeline_desc.DSVFormat = static_cast<DXGI_FORMAT>(render_pass_desc.depth_stencil.format);
        graphics_pipeline_desc.SampleDesc = sample_desc;
        
        ASSERT_SUCCEEDED(m_device.get()->CreateGraphicsPipelineState(&graphics_pipeline_desc, __uuidof(ID3D12PipelineState), m_d3d12_pipeline_state.put_void()));
    }

    PipelineType get_type() const override { return m_pipeline_type; }

    winrt::com_ptr<ID3D12RootSignature>& get_d3d12_root_signature() { return m_d3d12_root_signature; }
    winrt::com_ptr<ID3D12PipelineState>& get_d3d12_pipeline_state() { return m_d3d12_pipeline_state; }

private:

    std::reference_wrapper<winrt::com_ptr<ID3D12Device4>> m_device;
    std::reference_wrapper<winrt::com_ptr<ID3D12RootSignature>> m_d3d12_root_signature;
    
    winrt::com_ptr<ID3D12PipelineState> m_d3d12_pipeline_state;

    PipelineType m_pipeline_type;
};

}