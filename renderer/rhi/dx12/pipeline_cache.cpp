// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "core/exception.hpp"
#include "pipeline_cache.hpp"
#include "precompiled.h"
#include "utils.hpp"

namespace ionengine::renderer::rhi
{
    auto compare_op_to_d3d12(CompareOp const compare_op) -> D3D12_COMPARISON_FUNC
    {
        switch (compare_op)
        {
            case CompareOp::Always:
                return D3D12_COMPARISON_FUNC_ALWAYS;
            case CompareOp::Equal:
                return D3D12_COMPARISON_FUNC_EQUAL;
            case CompareOp::Greater:
                return D3D12_COMPARISON_FUNC_GREATER;
            case CompareOp::GreaterEqual:
                return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
            case CompareOp::Less:
                return D3D12_COMPARISON_FUNC_LESS;
            case CompareOp::LessEqual:
                return D3D12_COMPARISON_FUNC_LESS_EQUAL;
            case CompareOp::Never:
                return D3D12_COMPARISON_FUNC_NEVER;
            case CompareOp::NotEqual:
                return D3D12_COMPARISON_FUNC_NOT_EQUAL;
            default:
                return D3D12_COMPARISON_FUNC_NEVER;
        }
    }

    auto blend_to_d3d12(Blend const blend) -> D3D12_BLEND
    {
        switch (blend)
        {
            case Blend::InvSrcAlpha:
                return D3D12_BLEND_INV_SRC_ALPHA;
            case Blend::One:
                return D3D12_BLEND_ONE;
            case Blend::SrcAlpha:
                return D3D12_BLEND_SRC_ALPHA;
            case Blend::Zero:
                return D3D12_BLEND_ZERO;
            case Blend::BlendFactor:
                return D3D12_BLEND_BLEND_FACTOR;
            default:
                return D3D12_BLEND_ZERO;
        }
    }

    auto ionengine::renderer::rhi::blend_op_to_d3d12(BlendOp const blend_op) -> D3D12_BLEND_OP
    {
        switch (blend_op)
        {
            case BlendOp::Add:
                return D3D12_BLEND_OP_ADD;
            case BlendOp::Max:
                return D3D12_BLEND_OP_MAX;
            case BlendOp::Min:
                return D3D12_BLEND_OP_MIN;
            case BlendOp::RevSubtract:
                return D3D12_BLEND_OP_REV_SUBTRACT;
            case BlendOp::Subtract:
                return D3D12_BLEND_OP_SUBTRACT;
            default:
                return D3D12_BLEND_OP_ADD;
        }
    }

    Pipeline::Pipeline(ID3D12Device4* device, ID3D12RootSignature* root_signature, DX12Shader& shader,
                       RasterizerStageInfo const& rasterizer, BlendColorInfo const& blend_color,
                       std::optional<DepthStencilStageInfo> const depth_stencil,
                       std::span<DXGI_FORMAT const> const render_target_formats, DXGI_FORMAT const depth_stencil_format,
                       ID3DBlob* blob)
        : root_signature(root_signature)
    {
        auto d3d12_pipeline_desc = D3D12_GRAPHICS_PIPELINE_STATE_DESC{};
        d3d12_pipeline_desc.pRootSignature = root_signature;
        for (auto const& [stage, data] : shader.get_stages())
        {
            switch (stage)
            {
                case shader_file::ShaderStageType::Vertex: {
                    d3d12_pipeline_desc.VS = data;

                    auto d3d12_input_layout = D3D12_INPUT_LAYOUT_DESC{};
                    d3d12_input_layout.pInputElementDescs = shader.get_inputs().data();
                    d3d12_input_layout.NumElements = static_cast<uint32_t>(shader.get_inputs().size());

                    d3d12_pipeline_desc.InputLayout = d3d12_input_layout;
                    break;
                }
                case shader_file::ShaderStageType::Pixel: {
                    d3d12_pipeline_desc.PS = data;

                    auto d3d12_render_target_blend = D3D12_RENDER_TARGET_BLEND_DESC{};
                    d3d12_render_target_blend.BlendEnable = blend_color.blend_enable;
                    d3d12_render_target_blend.SrcBlend = blend_to_d3d12(blend_color.blend_src);
                    d3d12_render_target_blend.DestBlend = blend_to_d3d12(blend_color.blend_dst);
                    d3d12_render_target_blend.BlendOp = blend_op_to_d3d12(blend_color.blend_op);
                    d3d12_render_target_blend.SrcBlendAlpha = blend_to_d3d12(blend_color.blend_src_alpha);
                    d3d12_render_target_blend.DestBlendAlpha = blend_to_d3d12(blend_color.blend_dst_alpha);
                    d3d12_render_target_blend.BlendOpAlpha = blend_op_to_d3d12(blend_color.blend_op_alpha);
                    d3d12_render_target_blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

                    auto d3d12_blend = D3D12_BLEND_DESC{};
                    d3d12_pipeline_desc.NumRenderTargets = static_cast<uint32_t>(render_target_formats.size());
                    for (uint32_t const i : std::views::iota(0u, render_target_formats.size()))
                    {
                        d3d12_pipeline_desc.RTVFormats[i] = render_target_formats[i];
                        d3d12_blend.RenderTarget[i] = d3d12_render_target_blend;
                    }

                    d3d12_pipeline_desc.BlendState = d3d12_blend;

                    d3d12_pipeline_desc.DSVFormat = depth_stencil_format;
                    break;
                }
            }
        }

        auto d3d12_rasterizer = D3D12_RASTERIZER_DESC{};
        switch (rasterizer.fill_mode)
        {
            case FillMode::Solid: {
                d3d12_rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
                break;
            }
            case FillMode::Wireframe: {
                d3d12_rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
                break;
            }
        }
        switch (rasterizer.cull_mode)
        {
            case CullMode::Back: {
                d3d12_rasterizer.CullMode = D3D12_CULL_MODE_BACK;
                break;
            }
            case CullMode::Front: {
                d3d12_rasterizer.CullMode = D3D12_CULL_MODE_FRONT;
                break;
            }
            case CullMode::None: {
                d3d12_rasterizer.CullMode = D3D12_CULL_MODE_NONE;
                break;
            }
        }
        d3d12_rasterizer.FrontCounterClockwise = true;
        d3d12_rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        d3d12_rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        d3d12_rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        d3d12_rasterizer.DepthClipEnable = true;
        d3d12_rasterizer.MultisampleEnable = false;
        d3d12_rasterizer.AntialiasedLineEnable = false;
        d3d12_rasterizer.ForcedSampleCount = 0;
        d3d12_rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        d3d12_pipeline_desc.RasterizerState = d3d12_rasterizer;

        d3d12_pipeline_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        d3d12_pipeline_desc.SampleMask = std::numeric_limits<uint32_t>::max();
        d3d12_pipeline_desc.SampleDesc.Count = 1;

        auto d3d12_depth_stencil_face = D3D12_DEPTH_STENCILOP_DESC{};
        d3d12_depth_stencil_face.StencilFailOp = D3D12_STENCIL_OP_KEEP;
        d3d12_depth_stencil_face.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
        d3d12_depth_stencil_face.StencilPassOp = D3D12_STENCIL_OP_KEEP;
        d3d12_depth_stencil_face.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

        auto d3d12_depth_stencil = D3D12_DEPTH_STENCIL_DESC{};
        auto depth_stencil_value = depth_stencil.value_or(DepthStencilStageInfo::Default());
        d3d12_depth_stencil.DepthFunc = compare_op_to_d3d12(depth_stencil_value.depth_func);
        d3d12_depth_stencil.DepthEnable = depth_stencil_value.depth_write;
        d3d12_depth_stencil.StencilEnable = depth_stencil_value.stencil_write;
        d3d12_depth_stencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        d3d12_depth_stencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
        d3d12_depth_stencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
        d3d12_depth_stencil.FrontFace = d3d12_depth_stencil_face;
        d3d12_depth_stencil.BackFace = d3d12_depth_stencil_face;

        d3d12_pipeline_desc.DepthStencilState = d3d12_depth_stencil;

        if (blob)
        {
            d3d12_pipeline_desc.CachedPSO.pCachedBlob = blob->GetBufferPointer();
            d3d12_pipeline_desc.CachedPSO.CachedBlobSizeInBytes = blob->GetBufferSize();
        }

        HRESULT result = device->CreateGraphicsPipelineState(&d3d12_pipeline_desc, __uuidof(ID3D12PipelineState),
                                                             pipeline_state.put_void());
        if (result == D3D12_ERROR_ADAPTER_NOT_FOUND | result == D3D12_ERROR_DRIVER_VERSION_MISMATCH |
            result == E_INVALIDARG)
        {
            d3d12_pipeline_desc.CachedPSO = {};
            THROW_IF_FAILED(device->CreateGraphicsPipelineState(&d3d12_pipeline_desc, __uuidof(ID3D12PipelineState),
                                                                pipeline_state.put_void()));
        }
        else
        {
            THROW_IF_FAILED(result);
        }
    }

    PipelineCache::PipelineCache(ID3D12Device4* device) : device(device)
    {
        D3D12_ROOT_PARAMETER1 d3d12_root_parameter = {};
        d3d12_root_parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        d3d12_root_parameter.Constants.ShaderRegister = 0;
        d3d12_root_parameter.Constants.RegisterSpace = 0;
        d3d12_root_parameter.Constants.Num32BitValues = 16;
        d3d12_root_parameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        auto d3d12_static_sampler = D3D12_STATIC_SAMPLER_DESC{};
        d3d12_static_sampler.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        d3d12_static_sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        d3d12_static_sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        d3d12_static_sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        d3d12_static_sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        d3d12_static_sampler.MaxAnisotropy = 2;
        d3d12_static_sampler.MaxLOD = D3D12_FLOAT32_MAX;
        d3d12_static_sampler.ShaderRegister = 0;
        d3d12_static_sampler.RegisterSpace = 0;
        d3d12_static_sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        auto d3d12_root_signature = D3D12_VERSIONED_ROOT_SIGNATURE_DESC{};
        d3d12_root_signature.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
        d3d12_root_signature.Desc_1_1 = {.NumParameters = 1,
                                         .pParameters = &d3d12_root_parameter,
                                         .NumStaticSamplers = 1,
                                         .pStaticSamplers = &d3d12_static_sampler,
                                         .Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                                                  D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
                                                  D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED};

        winrt::com_ptr<ID3DBlob> blob;
        THROW_IF_FAILED(::D3D12SerializeVersionedRootSignature(&d3d12_root_signature, blob.put(), nullptr));
        THROW_IF_FAILED(device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
                                                    __uuidof(ID3D12RootSignature), root_signature.put_void()));

        /*
        TODO! Pipeline cache mechanism

        {
            HRESULT result = E_INVALIDARG;
            std::basic_ifstream<uint8_t> ifs("shader_cache.bin", std::ios::binary);
            if(ifs.is_open()) {
                std::vector<uint8_t> cache_bytes;
                ifs.seekg(0, std::ios::end);
                size_t size = ifs.tellg();
                ifs.seekg(0, std::ios::beg);
                cache_bytes.resize(size);
                ifs.read(cache_bytes.data(), cache_bytes.size());

                result = device->CreatePipelineLibrary(cache_bytes.data(), cache_bytes.size(),
        __uuidof(ID3D12PipelineLibrary), pipeline_library.put_void());
            }

            if(result == D3D12_ERROR_ADAPTER_NOT_FOUND | result == D3D12_ERROR_DRIVER_VERSION_MISMATCH | result ==
        E_INVALIDARG) { THROW_IF_FAILED(device->CreatePipelineLibrary(nullptr, 0, __uuidof(ID3D12PipelineLibrary),
        pipeline_library.put_void())); } else { THROW_IF_FAILED(result);
            }
        }

        */
    }

    PipelineCache::~PipelineCache()
    {

        /*
        TODO! Pipeline cache mechanism

        std::vector<uint8_t> cache_bytes(pipeline_library->GetSerializedSize());
        auto result = pipeline_library->Serialize(cache_bytes.data(), cache_bytes.size());
        if(result != E_FAIL) {
            std::basic_ofstream<uint8_t> ofs("shaders_cache.bin", std::ios::binary);
            ofs.write(cache_bytes.data(), cache_bytes.size());
        }

        */
    }

    auto PipelineCache::get(DX12Shader& shader, RasterizerStageInfo const& rasterizer,
                            BlendColorInfo const& blend_color, std::optional<DepthStencilStageInfo> const depth_stencil)
        -> core::ref_ptr<Pipeline>
    {
        auto entry = Entry{shader.get_hash(), rasterizer, blend_color, depth_stencil};

        std::unique_lock lock(mutex);
        auto result = entries.find(entry);
        if (result != entries.end())
        {
            return result->second;
        }
        else
        {
            lock.unlock();
            auto pipeline = core::make_ref<Pipeline>(device, root_signature.get(), shader, rasterizer, blend_color,
                                                     depth_stencil, nullptr);

            lock.lock();
            entries.emplace(entry, pipeline);
            return pipeline;
        }
    }

    auto PipelineCache::reset() -> void
    {
        std::lock_guard lock(mutex);
        entries.clear();
    }
} // namespace ionengine::renderer::rhi