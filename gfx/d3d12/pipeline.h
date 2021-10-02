// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class D3DPipeline : public Pipeline {
public:

    D3DPipeline(ID3D12Device4* d3d12_device, const GraphicsPipelineDesc& pipeline_desc) : m_type(PipelineType::Graphics) {

        // Input Element Description
        // Description of the input data in the pipeline
        std::vector<D3D12_INPUT_ELEMENT_DESC> input_descs;
        for(auto& vertex_input : pipeline_desc.vertex_inputs) {
            D3D12_INPUT_ELEMENT_DESC input_desc{};
            input_desc.SemanticName = vertex_input.semantic_name.c_str();
            input_desc.SemanticIndex = vertex_input.index;
            input_desc.Format = dxgi_format_to_gfx_enum(vertex_input.format);
            input_desc.InputSlot = vertex_input.slot;
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
        rasterizer_desc.FillMode = d3d12_fill_mode_to_gfx_enum(pipeline_desc.rasterizer.fill_mode);
        rasterizer_desc.CullMode = d3d12_cull_mode_to_gfx_enum(pipeline_desc.rasterizer.cull_mode);
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
        front_face_desc.StencilFailOp = d3d12_stencil_op_to_gfx_enum(pipeline_desc.depth_stencil.front_face.fail_op);
        front_face_desc.StencilPassOp = d3d12_stencil_op_to_gfx_enum(pipeline_desc.depth_stencil.front_face.pass_op);
        front_face_desc.StencilDepthFailOp = d3d12_stencil_op_to_gfx_enum(pipeline_desc.depth_stencil.front_face.depth_fail_op);
        front_face_desc.StencilFunc = d3d12_comparison_func_to_gfx_enum(pipeline_desc.depth_stencil.front_face.func);

        D3D12_DEPTH_STENCILOP_DESC back_face_desc{};
        back_face_desc.StencilFailOp = d3d12_stencil_op_to_gfx_enum(pipeline_desc.depth_stencil.back_face.fail_op);
        back_face_desc.StencilPassOp = d3d12_stencil_op_to_gfx_enum(pipeline_desc.depth_stencil.back_face.pass_op);
        back_face_desc.StencilDepthFailOp = d3d12_stencil_op_to_gfx_enum(pipeline_desc.depth_stencil.back_face.depth_fail_op);
        back_face_desc.StencilFunc = d3d12_comparison_func_to_gfx_enum(pipeline_desc.depth_stencil.back_face.func);

        D3D12_DEPTH_STENCIL_DESC depth_stencil_desc{};
        depth_stencil_desc.DepthEnable = pipeline_desc.depth_stencil.depth_test_enable;
        depth_stencil_desc.DepthWriteMask = pipeline_desc.depth_stencil.depth_write_enable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
        // depth_stencil_desc.DepthBoundsTestEnable = desc.depth_stencil.depth_bounds_test_enable;
        depth_stencil_desc.DepthFunc = d3d12_comparison_func_to_gfx_enum(pipeline_desc.depth_stencil.depth_func);
        depth_stencil_desc.StencilEnable = pipeline_desc.depth_stencil.stencil_enable;
        depth_stencil_desc.StencilReadMask = pipeline_desc.depth_stencil.stencil_read_mask;
        depth_stencil_desc.StencilWriteMask = pipeline_desc.depth_stencil.stencil_write_mask;
        depth_stencil_desc.FrontFace = front_face_desc;
        depth_stencil_desc.BackFace = back_face_desc;

        // Get Render Pass Description
        auto& render_pass_desc = pipeline_desc.render_pass->get_desc();

        // Blend Description
        // Description of pipeline blend
        D3D12_BLEND_DESC blend_desc{};
        for(uint32 i = 0; i < render_pass_desc.colors.size(); ++i) {
            if(render_pass_desc.colors[i].format == Format::Unknown) {
                continue;
            }
            auto& render_target = blend_desc.RenderTarget[i];

            render_target.BlendEnable = pipeline_desc.blend.blend_enable;
            render_target.BlendOp = d3d12_blend_op_to_gfx_enum(pipeline_desc.blend.blend_op);
            render_target.SrcBlend = d3d12_blend_to_gfx_enum(pipeline_desc.blend.blend_src);
            render_target.DestBlend = d3d12_blend_to_gfx_enum(pipeline_desc.blend.blend_dest);
            render_target.BlendOpAlpha = d3d12_blend_op_to_gfx_enum(pipeline_desc.blend.blend_op_alpha);
            render_target.SrcBlendAlpha = d3d12_blend_to_gfx_enum(pipeline_desc.blend.blend_src_alpha);
            render_target.DestBlendAlpha = d3d12_blend_to_gfx_enum(pipeline_desc.blend.blend_dest_alpha);
            render_target.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        }

        // Render Target Formats
        std::vector<DXGI_FORMAT> rtv_formats;
        rtv_formats.resize(render_pass_desc.colors.size());
        for(uint32 i = 0; i < render_pass_desc.colors.size(); ++i) {
            if(render_pass_desc.colors[i].format == Format::Unknown) {
                continue;
            }
            rtv_formats[i] = dxgi_format_to_gfx_enum(render_pass_desc.colors[i].format);
        }

        // Sample Description
        DXGI_SAMPLE_DESC sample_desc{};
        sample_desc.Count = render_pass_desc.sample_count;

        // Graphics Pipeline State Description
        // Description of the pipeline
        D3D12_GRAPHICS_PIPELINE_STATE_DESC graphics_pipeline_desc{};
        graphics_pipeline_desc.pRootSignature = static_cast<D3DBindingSetLayout*>(pipeline_desc.layout)->get_d3d12_root_signature();
        graphics_pipeline_desc.InputLayout = input_layout_desc;
        graphics_pipeline_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        graphics_pipeline_desc.RasterizerState = rasterizer_desc;
        graphics_pipeline_desc.DepthStencilState = depth_stencil_desc;
        graphics_pipeline_desc.BlendState = blend_desc;

        
        auto read_shader_file = [](const std::filesystem::path& shader_path) -> lib::Expected<std::vector<byte>, std::string> {

            std::vector<byte> shader_blob;

            std::ifstream ifs(shader_path, std::ios::binary);
            if(!ifs.is_open()) {
                return lib::make_expected<std::vector<byte>, std::string>("Shader error when opening file");
            }

            ifs.seekg(0, std::ios::end);
            usize size = ifs.tellg();
            ifs.seekg(0, std::ios::beg);

            shader_blob.resize(size);
            ifs.read(reinterpret_cast<char*>(shader_blob.data()), shader_blob.size());
            return lib::make_expected<std::vector<byte>, std::string>(shader_blob);
        };

        std::vector<std::vector<byte>> shader_blobs;
        for(auto& shader : pipeline_desc.shaders) {

            auto result = read_shader_file(shader.blob_path);
            std::visit(
                lib::default_visitor {
                    [&](const std::vector<byte>& arg) {

                        auto& blob = shader_blobs.emplace_back(arg);

                        D3D12_SHADER_BYTECODE shader_code{};
                        shader_code.pShaderBytecode = blob.data();
                        shader_code.BytecodeLength = blob.size();

                        switch(shader.shader_type) {
                            case ShaderType::Vertex: graphics_pipeline_desc.VS = shader_code; break;
                            case ShaderType::Pixel: graphics_pipeline_desc.PS = shader_code; break;
                            case ShaderType::Geometry: graphics_pipeline_desc.GS = shader_code; break;
                            case ShaderType::Domain: graphics_pipeline_desc.DS = shader_code; break;
                            case ShaderType::Hull: graphics_pipeline_desc.HS = shader_code; break;
                        }
                    },
                    [&](const std::string& arg) {
                        throw std::runtime_error("Pipeline creation error (" + arg + ")");
                    }
                },
                result.get()
            );
        }

        std::memcpy(graphics_pipeline_desc.RTVFormats, rtv_formats.data(), rtv_formats.size() * sizeof(DXGI_FORMAT));
        graphics_pipeline_desc.NumRenderTargets = static_cast<uint32>(rtv_formats.size());
        graphics_pipeline_desc.DSVFormat = static_cast<DXGI_FORMAT>(render_pass_desc.depth_stencil.format);
        graphics_pipeline_desc.SampleDesc = sample_desc;
        
        THROW_IF_FAILED(d3d12_device->CreateGraphicsPipelineState(&graphics_pipeline_desc, __uuidof(ID3D12PipelineState), m_d3d12_pipeline_state.put_void()));
    }

    D3DPipeline(ID3D12Device4* d3d12_device, const ComputePipelineDesc& pipeline_desc) : m_type(PipelineType::Compute) {

    }

    PipelineType get_type() const override { return m_type; }

    ID3D12PipelineState* get_d3d12_pipeline_state() { return m_d3d12_pipeline_state.get(); }

private:
    
    winrt::com_ptr<ID3D12PipelineState> m_d3d12_pipeline_state;

    PipelineType m_type;
};

}