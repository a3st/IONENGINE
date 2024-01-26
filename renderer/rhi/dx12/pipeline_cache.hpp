// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/ref_ptr.hpp"
#include "renderer/rhi/command_buffer.hpp"
#include "shader.hpp"
#define NOMINMAX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <winrt/base.h>
#include <xxhash/xxhash64.h>

namespace ionengine {

namespace renderer {

namespace rhi {

auto compare_op_to_d3d12(CompareOp const compare_op) -> D3D12_COMPARISON_FUNC;

auto blend_to_d3d12(Blend const blend) -> D3D12_BLEND;

auto blend_op_to_d3d12(BlendOp const blend_op) -> D3D12_BLEND_OP;

class Pipeline : public core::ref_counted_object {
public:

    Pipeline(
        ID3D12Device4* device, 
        ID3D12RootSignature* root_signature, 
        DX12Shader& shader, 
        RasterizerStageInfo const& rasterizer,
        BlendColorInfo const& blend_color,
        std::optional<DepthStencilStageInfo> const depth_stencil,
        ID3DBlob* blob
    );

    auto get_pipeline_state() -> ID3D12PipelineState* {

        return pipeline_state.get();
    }

    auto get_root_signature() -> ID3D12RootSignature* {

        return root_signature;
    }

private:

    ID3D12RootSignature* root_signature;
    winrt::com_ptr<ID3D12PipelineState> pipeline_state;
};

class PipelineCache : public core::ref_counted_object {
public:

    struct Entry {
        uint64_t shader_hash;
        RasterizerStageInfo rasterizer;
        BlendColorInfo blend_color;
        std::optional<DepthStencilStageInfo> depth_stencil;

        auto operator==(Entry const& other) const -> bool {
            return std::make_tuple(
                shader_hash, 
                rasterizer.fill_mode,
                rasterizer.cull_mode,
                blend_color.blend_dst,
                blend_color.blend_dst_alpha,
                blend_color.blend_enable,
                blend_color.blend_op,
                blend_color.blend_op_alpha,
                blend_color.blend_src,
                blend_color.blend_src_alpha,
                depth_stencil.value_or(DepthStencilStageInfo::Default()).depth_func,
                depth_stencil.value_or(DepthStencilStageInfo::Default()).write_enable
            ) == std::make_tuple(
                other.shader_hash, 
                other.rasterizer.fill_mode,
                other.rasterizer.cull_mode,
                other.blend_color.blend_dst,
                other.blend_color.blend_dst_alpha,
                other.blend_color.blend_enable,
                other.blend_color.blend_op,
                other.blend_color.blend_op_alpha,
                other.blend_color.blend_src,
                other.blend_color.blend_src_alpha,
                other.depth_stencil.value_or(DepthStencilStageInfo::Default()).depth_func,
                other.depth_stencil.value_or(DepthStencilStageInfo::Default()).write_enable
            );
        }
    };

    struct EntryHasher {
        auto operator()(const Entry& entry) const -> std::size_t {
            auto depth_stencil = entry.depth_stencil.value_or(DepthStencilStageInfo::Default());
            return 
                entry.shader_hash ^
                XXHash64::hash(&entry.rasterizer.fill_mode, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.rasterizer.cull_mode, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.blend_color.blend_dst, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.blend_color.blend_dst_alpha, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.blend_color.blend_enable, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.blend_color.blend_op, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.blend_color.blend_op_alpha, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.blend_color.blend_src, sizeof(uint32_t), 0) ^
                XXHash64::hash(&entry.blend_color.blend_src_alpha, sizeof(uint32_t), 0) ^
                XXHash64::hash(&depth_stencil.depth_func, sizeof(uint32_t), 0) ^
                XXHash64::hash(&depth_stencil.write_enable, sizeof(uint32_t), 0)
            ;
        }
    };

    PipelineCache(ID3D12Device4* device);

    ~PipelineCache();

    auto get(
        DX12Shader& shader, 
        RasterizerStageInfo const& rasterizer,
        BlendColorInfo const& blend_color,
        std::optional<DepthStencilStageInfo> const depth_stencil
    ) -> core::ref_ptr<Pipeline>;

    auto reset() -> void;

private:

    ID3D12Device4* device;
    winrt::com_ptr<ID3D12RootSignature> root_signature;
    std::unordered_map<Entry, core::ref_ptr<Pipeline>, EntryHasher> entries;
    // winrt::com_ptr<ID3D12PipelineLibrary> pipeline_library;
};

}

}

}