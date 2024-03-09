// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "core/exception.hpp"
#include "rhi/rhi.hpp"
#include "rhi/shader_file.hpp"
#include <xxhash/xxhash64.h>
#define NOMINMAX
#include <D3D12MemAlloc.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#ifndef THROW_IF_FAILED
#define THROW_IF_FAILED(Result)                                                                                        \
    if (FAILED(Result))                                                                                                \
        throw ionengine::core::Exception(ionengine::rhi::hresult_to_string(Result));
#endif // THROW_IF_FAILED

namespace ionengine::rhi
{
    inline auto hresult_to_string(HRESULT const result) -> std::string
    {
        switch (result)
        {
            case E_FAIL:
                return "Attempted to create a device with the debug layer enabled and the layer is not "
                       "installed";
            case E_INVALIDARG:
                return "An invalid parameter was passed to the returning function";
            case E_OUTOFMEMORY:
                return "Direct3D could not allocate sufficient memory to complete the call";
            case E_NOTIMPL:
                return "The method call isn't implemented with the passed parameter combination";
            case S_FALSE:
                return "Alternate success value, indicating a successful but nonstandard completion";
            case S_OK:
                return "No error occurred";
            case D3D12_ERROR_ADAPTER_NOT_FOUND:
                return "The specified cached PSO was created on a different adapter and cannot be "
                       "reused on the current adapter";
            case D3D12_ERROR_DRIVER_VERSION_MISMATCH:
                return "The specified cached PSO was created on a different driver version and cannot "
                       "be reused on the current adapter";
            case DXGI_ERROR_INVALID_CALL:
                return "The method call is invalid. For example, a method's parameter may not be a "
                       "valid pointer";
            case DXGI_ERROR_WAS_STILL_DRAWING:
                return "The previous blit operation that is transferring information to or from this "
                       "surface is incomplete";
            default:
                return "An unknown error has occurred";
        }
    }

    struct DescriptorAllocation
    {
        ID3D12DescriptorHeap* heap;
        D3D12_DESCRIPTOR_HEAP_TYPE heap_type;
        uint32_t increment_size;
        uint32_t offset;
        uint32_t size;

        auto cpu_handle(uint32_t const index = 0) const -> D3D12_CPU_DESCRIPTOR_HANDLE
        {
            return {.ptr = heap->GetCPUDescriptorHandleForHeapStart().ptr + increment_size * offset + index};
        }

        auto gpu_handle(uint32_t const index = 0) const -> D3D12_GPU_DESCRIPTOR_HANDLE
        {
            return {.ptr = heap->GetGPUDescriptorHandleForHeapStart().ptr + increment_size * offset + index};
        }
    };

    inline uint32_t constexpr DX12_DESCRIPTOR_ALLOCATOR_RTV_MAX = 128;
    inline uint32_t constexpr DX12_DESCRIPTOR_ALLOCATOR_CBV_SRV_UAV_MAX = 16 * 1024;
    inline uint32_t constexpr DX12_DESCRIPTOR_ALLOCATOR_SAMPLER_MAX = 128;
    inline uint32_t constexpr DX12_DESCRIPTOR_ALLOCATOR_DSV_MAX = 32;

    class DescriptorAllocator : public core::ref_counted_object
    {
      public:
        DescriptorAllocator(ID3D12Device1* device);

        auto allocate(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type, uint32_t const size) -> DescriptorAllocation;

        auto deallocate(DescriptorAllocation const& allocation) -> void;

        auto get_heap(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type) -> ID3D12DescriptorHeap*
        {
            return chunks[heap_type].heap.get();
        }

      private:
        struct Chunk
        {
            winrt::com_ptr<ID3D12DescriptorHeap> heap;
            std::vector<uint8_t> free;
            uint32_t offset;
            uint32_t size;
        };

        ID3D12Device1* device;
        std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, Chunk> chunks;

        auto create_chunk(D3D12_DESCRIPTOR_HEAP_TYPE const heap_type) -> void;
    };

    struct MemoryAllocation
    {
        ID3D12Heap* heap;
        uint64_t alignment;
        uint64_t offset;
        size_t size;
    };

    inline uint32_t constexpr DX12_MEMORY_ALLOCATOR_CHUNK_MAX_SIZE = 256 * 1024 * 1024;

    class MemoryAllocator : public core::ref_counted_object
    {
      public:
        MemoryAllocator(ID3D12Device1* device, size_t const block_size, size_t const chunk_size);

        auto allocate(D3D12_RESOURCE_DESC const& resource_desc, D3D12_HEAP_TYPE const heap_type) -> MemoryAllocation;

        auto deallocate(MemoryAllocation const& allocation) -> void;

        auto reset() -> void;

      private:
        struct Chunk
        {
            winrt::com_ptr<ID3D12Heap> heap;
            std::vector<uint8_t> free;
            uint64_t offset;
            size_t size;
        };

        std::mutex mutex;
        ID3D12Device1* device;

        using KeyChunk = std::pair<uint64_t, D3D12_HEAP_TYPE>;

        struct KeyChunkHasher
        {
            auto operator()(const KeyChunk& key) const -> std::size_t
            {
                return std::hash<uint64_t>()(key.first) ^ std::hash<uint32_t>()(key.second);
            }
        };

        std::unordered_map<KeyChunk, std::vector<Chunk>, KeyChunkHasher> chunks;
        std::unordered_map<uintptr_t, uint32_t> ptr_chunks;
        size_t block_size;
        size_t chunk_size;

        auto create_chunk(uint64_t const alignment, D3D12_HEAP_TYPE const heap_type) -> void;
    };

    inline uint32_t constexpr DX12_CONSTANT_BUFFER_SIZE_ALIGNMENT = 256;

    class DX12Buffer final : public Buffer
    {
      public:
        DX12Buffer(ID3D12Device1* device, D3D12MA::Allocator* memory_allocator,
                   DescriptorAllocator* descriptor_allocator, size_t const size, size_t const element_stride,
                   BufferUsageFlags const flags);

        ~DX12Buffer();

        auto get_size() -> size_t override
        {
            return size;
        }

        auto get_flags() -> BufferUsageFlags override
        {
            return flags;
        }

        auto map_memory() -> uint8_t* override;

        auto unmap_memory() -> void override;

        auto get_resource() -> ID3D12Resource*
        {
            return resource.get();
        }

        auto get_descriptor(BufferUsage const usage) const -> DescriptorAllocation const&
        {
            return descriptor_allocations.at(usage);
        }

        auto get(BufferUsage const usage) const -> uint32_t override
        {
            return descriptor_allocations.at(usage).offset;
        }

      private:
        D3D12MA::Allocator* memory_allocator;
        DescriptorAllocator* descriptor_allocator;
        winrt::com_ptr<ID3D12Resource> resource;
        winrt::com_ptr<D3D12MA::Allocation> memory_allocation;
        std::unordered_map<BufferUsage, DescriptorAllocation> descriptor_allocations;
        size_t size;
        BufferUsageFlags flags;
    };

    auto dxgi_to_texture_format(DXGI_FORMAT const format) -> TextureFormat;

    auto texture_format_to_dxgi(TextureFormat const format) -> DXGI_FORMAT;

    class DX12Texture final : public Texture
    {
      public:
        DX12Texture(ID3D12Device1* device, D3D12MA::Allocator* memory_allocator,
                    DescriptorAllocator& descriptor_allocator, uint32_t const width, uint32_t const height,
                    uint32_t const depth, uint32_t const mip_levels, TextureFormat const format,
                    TextureDimension const dimension, TextureUsageFlags const flags);

        DX12Texture(ID3D12Device1* device, winrt::com_ptr<ID3D12Resource> resource,
                    DescriptorAllocator& descriptor_allocator, TextureUsageFlags const flags);

        ~DX12Texture();

        auto get_width() const -> uint32_t override
        {
            return width;
        }

        auto get_height() const -> uint32_t override
        {
            return height;
        }

        auto get_depth() const -> uint32_t override
        {
            return depth;
        }

        auto get_mip_levels() const -> uint32_t override
        {
            return mip_levels;
        }

        auto get_format() const -> TextureFormat override
        {
            return format;
        }

        auto get_flags() const -> TextureUsageFlags override
        {
            return flags;
        }

        auto get_descriptor(TextureUsage const usage) const -> DescriptorAllocation const&
        {
            return descriptor_allocations.at(usage);
        }

        auto get_resource() -> ID3D12Resource*
        {
            return resource.get();
        }

        auto get(TextureUsage const usage) const -> uint32_t override
        {
            return descriptor_allocations.at(usage).offset;
        }

      private:
        D3D12MA::Allocator* memory_allocator;
        DescriptorAllocator* descriptor_allocator;
        winrt::com_ptr<ID3D12Resource> resource;
        winrt::com_ptr<D3D12MA::Allocation> memory_allocation;
        std::unordered_map<TextureUsage, DescriptorAllocation> descriptor_allocations;
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t mip_levels;
        TextureFormat format;
        TextureDimension dimension;
        TextureUsageFlags flags;
    };

    auto element_type_to_dxgi(rhi::shaderfile::ElementType const element_type) -> DXGI_FORMAT;

    auto element_type_size(rhi::shaderfile::ElementType const element_type) -> uint32_t;

    class DX12Shader final : public Shader
    {
      public:
        DX12Shader(ID3D12Device4* device, std::span<uint8_t const> const data);

        auto get_name() const -> std::string_view override;

        auto get_inputs() const -> std::span<D3D12_INPUT_ELEMENT_DESC const>;

        auto get_inputs_size_per_vertex() const -> uint32_t;

        auto get_stages() const -> std::unordered_map<shaderfile::ShaderStageType, D3D12_SHADER_BYTECODE> const&;

        auto get_bindings() const -> std::unordered_map<std::string, shaderfile::ResourceData> const& override;

        auto get_hash() const -> uint64_t;

      private:
        std::string shader_name;
        std::unordered_map<rhi::shaderfile::ShaderStageType, D3D12_SHADER_BYTECODE> stages;
        std::vector<D3D12_INPUT_ELEMENT_DESC> inputs;
        uint32_t inputs_size_per_vertex;
        std::list<std::string> semantic_names;
        std::vector<std::vector<uint8_t>> buffers;
        std::unordered_map<std::string, shaderfile::ResourceData> bindings;
        uint64_t hash;
    };

    auto compare_op_to_d3d12(CompareOp const compare_op) -> D3D12_COMPARISON_FUNC;

    auto blend_to_d3d12(Blend const blend) -> D3D12_BLEND;

    auto blend_op_to_d3d12(BlendOp const blend_op) -> D3D12_BLEND_OP;

    class Pipeline final : public core::ref_counted_object
    {
      public:
        Pipeline(ID3D12Device4* device, ID3D12RootSignature* root_signature, DX12Shader& shader,
                 RasterizerStageInfo const& rasterizer, BlendColorInfo const& blend_color,
                 std::optional<DepthStencilStageInfo> const depth_stencil,
                 std::span<DXGI_FORMAT const> const render_target_formats, DXGI_FORMAT const depth_stencil_format,
                 ID3DBlob* blob);

        auto get_pipeline_state() -> ID3D12PipelineState*
        {
            return pipeline_state.get();
        }

        auto get_root_signature() -> ID3D12RootSignature*
        {
            return root_signature;
        }

      private:
        ID3D12RootSignature* root_signature;
        winrt::com_ptr<ID3D12PipelineState> pipeline_state;
    };

    class PipelineCache final : public core::ref_counted_object
    {
      public:
        struct Entry
        {
            uint64_t shader_hash;
            RasterizerStageInfo rasterizer;
            BlendColorInfo blend_color;
            std::optional<DepthStencilStageInfo> depth_stencil;
            std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> render_target_formats;
            DXGI_FORMAT depth_stencil_format;

            auto operator==(Entry const& other) const -> bool
            {
                return std::make_tuple(shader_hash, rasterizer.fill_mode, rasterizer.cull_mode, blend_color.blend_dst,
                                       blend_color.blend_dst_alpha, blend_color.blend_enable, blend_color.blend_op,
                                       blend_color.blend_op_alpha, blend_color.blend_src, blend_color.blend_src_alpha,
                                       depth_stencil.value_or(DepthStencilStageInfo::Default()).depth_func,
                                       depth_stencil.value_or(DepthStencilStageInfo::Default()).depth_write,
                                       depth_stencil.value_or(DepthStencilStageInfo::Default()).stencil_write,
                                       render_target_formats, depth_stencil_format) ==
                       std::make_tuple(other.shader_hash, other.rasterizer.fill_mode, other.rasterizer.cull_mode,
                                       other.blend_color.blend_dst, other.blend_color.blend_dst_alpha,
                                       other.blend_color.blend_enable, other.blend_color.blend_op,
                                       other.blend_color.blend_op_alpha, other.blend_color.blend_src,
                                       other.blend_color.blend_src_alpha,
                                       other.depth_stencil.value_or(DepthStencilStageInfo::Default()).depth_func,
                                       other.depth_stencil.value_or(DepthStencilStageInfo::Default()).depth_write,
                                       other.depth_stencil.value_or(DepthStencilStageInfo::Default()).stencil_write,
                                       other.render_target_formats, other.depth_stencil_format);
            }
        };

        struct EntryHasher
        {
            auto operator()(const Entry& entry) const -> std::size_t
            {
                auto depth_stencil = entry.depth_stencil.value_or(DepthStencilStageInfo::Default());
                return entry.shader_hash ^ XXHash64::hash(&entry.rasterizer.fill_mode, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&entry.rasterizer.cull_mode, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&entry.blend_color.blend_dst, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&entry.blend_color.blend_dst_alpha, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&entry.blend_color.blend_enable, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&entry.blend_color.blend_op, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&entry.blend_color.blend_op_alpha, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&entry.blend_color.blend_src, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&entry.blend_color.blend_src_alpha, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&depth_stencil.depth_func, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&depth_stencil.depth_write, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&depth_stencil.stencil_write, sizeof(uint32_t), 0) ^
                       XXHash64::hash(&entry.render_target_formats, entry.render_target_formats.size(), 0) ^
                       XXHash64::hash(&entry.depth_stencil_format, sizeof(DXGI_FORMAT), 0);
            }
        };

        PipelineCache(ID3D12Device4* device);

        auto get(DX12Shader& shader, RasterizerStageInfo const& rasterizer, BlendColorInfo const& blend_color,
                 std::optional<DepthStencilStageInfo> const depth_stencil,
                 std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> const& render_target_formats,
                 DXGI_FORMAT const depth_stencil_format) -> core::ref_ptr<Pipeline>;

        auto reset() -> void;

      private:
        std::mutex mutex;
        ID3D12Device4* device;
        winrt::com_ptr<ID3D12RootSignature> root_signature;
        std::unordered_map<Entry, core::ref_ptr<Pipeline>, EntryHasher> entries;
    };

    auto render_pass_load_to_d3d12(RenderPassLoadOp const load_op) -> D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE;

    auto render_pass_store_to_d3d12(RenderPassStoreOp const store_op) -> D3D12_RENDER_PASS_ENDING_ACCESS_TYPE;

    auto resource_state_to_d3d12(ResourceState const state) -> D3D12_RESOURCE_STATES;

    class DX12Query final : public Query
    {
    };

    class DX12GraphicsContext final : public GraphicsContext
    {
      public:
        DX12GraphicsContext(ID3D12Device4* device, PipelineCache& pipeline_cache,
                            DescriptorAllocator& descriptor_allocator, ID3D12CommandQueue* queue, ID3D12Fence* fence,
                            HANDLE fence_event, uint64_t& fence_value);

        auto reset() -> void override;

        auto set_graphics_pipeline_options(core::ref_ptr<Shader> shader, RasterizerStageInfo const& rasterizer,
                                           BlendColorInfo const& blend_color,
                                           std::optional<DepthStencilStageInfo> const depth_stencil) -> void override;

        auto bind_descriptor(std::string_view const binding, uint32_t const descriptor) -> void override;

        auto begin_render_pass(std::span<RenderPassColorInfo> const colors,
                               std::optional<RenderPassDepthStencilInfo> depth_stencil) -> void override;

        auto end_render_pass() -> void override;

        auto bind_vertex_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size)
            -> void override;

        auto bind_index_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                               IndexFormat const format) -> void override;

        auto draw_indexed(uint32_t const index_count, uint32_t const instance_count) -> void override;

        auto draw(uint32_t const vertex_count, uint32_t const instance_count) -> void override;

        auto set_viewport(int32_t const x, int32_t const y, uint32_t const width, uint32_t const height)
            -> void override;

        auto set_scissor(int32_t const left, int32_t const top, int32_t const right, int32_t const bottom)
            -> void override;

        auto barrier(std::variant<core::ref_ptr<Buffer>, core::ref_ptr<Texture>> dst, ResourceState const before,
                     ResourceState const after) -> void override;

        auto execute() -> Future<Query> override;

      private:
        winrt::com_ptr<ID3D12CommandAllocator> command_allocator;
        winrt::com_ptr<ID3D12GraphicsCommandList4> command_list;

        PipelineCache* pipeline_cache;
        DescriptorAllocator* descriptor_allocator;
        ID3D12CommandQueue* queue;
        ID3D12Fence* fence;
        HANDLE fence_event;
        uint64_t* fence_value;

        core::ref_ptr<DX12Shader> current_shader;
        bool is_root_signature_binded;
        std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> render_target_formats;
        DXGI_FORMAT depth_stencil_format;
        std::array<uint32_t, 16> bindings;
    };

    inline uint32_t constexpr DX12_RESOURCE_MEMORY_ALIGNMENT = 256;
    inline uint32_t constexpr DX12_TEXTURE_ROW_PITH_ALIGNMENT = 512;

    auto get_surface_data(rhi::TextureFormat const format, uint32_t const width, uint32_t const height,
                          size_t& row_bytes, uint32_t& row_count) -> void;

    class DX12CopyContext final : public CopyContext
    {
      public:
        DX12CopyContext(ID3D12Device4* device, D3D12MA::Allocator* memory_allocator, ID3D12CommandQueue* queue,
                        ID3D12Fence* fence, HANDLE fence_event, uint64_t& fence_value);

        auto reset() -> void override;

        auto write_buffer(core::ref_ptr<Buffer> dst, std::span<uint8_t const> const data) -> Future<Buffer> override;

        auto write_texture(core::ref_ptr<Texture> dst, std::vector<std::span<uint8_t const>> const& data)
            -> Future<Texture> override;

        auto read_buffer(core::ref_ptr<Buffer> dst, std::vector<uint8_t>& data) -> void override;

        auto read_texture(core::ref_ptr<Texture> dst, std::vector<std::vector<uint8_t>>& data) -> void override;

        auto barrier(std::variant<core::ref_ptr<Buffer>, core::ref_ptr<Texture>> dst, ResourceState const before,
                     ResourceState const after) -> void override;

        auto execute() -> Future<Query> override;

      private:
        ID3D12Device4* device;
        winrt::com_ptr<ID3D12CommandAllocator> command_allocator;
        winrt::com_ptr<ID3D12GraphicsCommandList4> command_list;

        ID3D12CommandQueue* queue;
        ID3D12Fence* fence;
        HANDLE fence_event;
        uint64_t* fence_value;

        core::ref_ptr<MemoryAllocator> memory_allocator;

        struct BufferInfo
        {
            core::ref_ptr<Buffer> buffer;
            uint64_t offset;
        };

        BufferInfo read_info;
        BufferInfo write_info;
    };

    class DX12FutureImpl final : public FutureImpl
    {
      public:
        DX12FutureImpl(ID3D12CommandQueue* queue, ID3D12Fence* fence, HANDLE fence_event, uint64_t const fence_value);

        auto get_result() const -> bool override;

        auto wait() -> void override;

      private:
        ID3D12CommandQueue* queue;
        ID3D12Fence* fence;
        HANDLE fence_event;
        uint64_t fence_value;
    };

    class DX12Device final : public Device
    {
      public:
        DX12Device(platform::Window* window);

        ~DX12Device();

        auto create_shader(std::span<uint8_t const> const data) -> core::ref_ptr<Shader> override;

        auto create_texture(uint32_t const width, uint32_t const height, uint32_t const depth,
                            uint32_t const mip_levels, TextureFormat const format, TextureDimension const dimension,
                            TextureUsageFlags const flags) -> core::ref_ptr<Texture> override;

        auto create_buffer(size_t const size, size_t const element_stride, BufferUsageFlags const flags)
            -> core::ref_ptr<Buffer> override;

        auto create_graphics_context() -> core::ref_ptr<GraphicsContext> override;

        auto create_copy_context() -> core::ref_ptr<CopyContext> override;

        auto request_back_buffer() -> core::ref_ptr<Texture> override;

        auto present_back_buffer() -> void override;

      private:
        std::mutex mutex;

#ifdef _DEBUG
        winrt::com_ptr<ID3D12Debug1> debug;
#endif
        winrt::com_ptr<IDXGIFactory4> factory;
        winrt::com_ptr<IDXGIAdapter1> adapter;
        winrt::com_ptr<ID3D12Device4> device;
        winrt::com_ptr<IDXGISwapChain3> swapchain;
        winrt::com_ptr<D3D12MA::Allocator> memory_allocator;

        struct QueueInfo
        {
            winrt::com_ptr<ID3D12CommandQueue> queue;
            winrt::com_ptr<ID3D12Fence> fence;
            uint64_t fence_value;
        };
        QueueInfo graphics_info;
        QueueInfo copy_info;
        QueueInfo compute_info;

        HANDLE fence_event;

        core::ref_ptr<DescriptorAllocator> descriptor_allocator;
        core::ref_ptr<PipelineCache> pipeline_cache;

        std::vector<core::ref_ptr<Texture>> back_buffers;
    };
} // namespace ionengine::rhi