// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "rhi/rhi.hpp"
#include <xxhash.h>
#define NOMINMAX
#include <D3D12MemAlloc.h>
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_4.h>
#include <winrt/base.h>

namespace ionengine::rhi
{
    class DescriptorAllocator;

    class DescriptorAllocation final : public IUnknown
    {
        friend class DescriptorAllocator;

      public:
        HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void** ppvObject) override;

        ULONG STDMETHODCALLTYPE AddRef() override;

        ULONG STDMETHODCALLTYPE Release() override;

        auto getCPUHandle() const -> D3D12_CPU_DESCRIPTOR_HANDLE;

        auto getGPUHandle() const -> D3D12_GPU_DESCRIPTOR_HANDLE;

        auto getOffset() const -> uint32_t;

      private:
        DescriptorAllocator* allocator;
        ID3D12DescriptorHeap* heap;
        D3D12_DESCRIPTOR_HEAP_TYPE heapType;
        uint32_t incrementSize;
        uint32_t offset;

        DescriptorAllocation() = default;

        auto initialize(DescriptorAllocator* allocator, ID3D12DescriptorHeap* heap,
                        D3D12_DESCRIPTOR_HEAP_TYPE const heapType, uint32_t const incrementSize,
                        uint32_t const offset) -> void;

        std::atomic<uint32_t> refCount;
    };

    enum class DescriptorAllocatorLimits : uint32_t
    {
        RTV = 128,
        CBV_SRV_UAV = 16 * 1024,
        Sampler = 128,
        DSV = 32
    };

    class DescriptorAllocator final : public core::ref_counted_object
    {
      public:
        DescriptorAllocator(ID3D12Device1* device);

        auto allocate(D3D12_DESCRIPTOR_HEAP_TYPE const heapType, DescriptorAllocation** allocation) -> HRESULT;

        auto deallocate(DescriptorAllocation* allocation) -> void;

        auto getDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE const heapType) -> ID3D12DescriptorHeap*;

      private:
        std::mutex mutex;
        ID3D12Device1* device;

        struct Chunk
        {
            winrt::com_ptr<ID3D12DescriptorHeap> heap;
            std::vector<uint8_t> free;
            uint32_t offset;
            uint32_t size;
            uint32_t incrementSize;
            std::unique_ptr<DescriptorAllocation[]> allocations;
        };

        std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, Chunk> chunks;
    };

    class DX12Buffer final : public Buffer
    {
      public:
        DX12Buffer(ID3D12Device1* device, D3D12MA::Allocator* memoryAllocator, DescriptorAllocator* descriptorAllocator,
                   BufferCreateInfo const& createInfo);

        auto getSize() -> size_t override;

        auto getFlags() -> BufferUsageFlags override;

        auto mapMemory() -> uint8_t* override;

        auto unmapMemory() -> void override;

        auto getDescriptorOffset(BufferUsage const usage) const -> uint32_t override;

        auto getResource() -> ID3D12Resource*;

        auto getDescriptor(BufferUsage const usage) const -> DescriptorAllocation*;

      private:
        D3D12MA::Allocator* memoryAllocator;
        DescriptorAllocator* descriptorAllocator;
        winrt::com_ptr<ID3D12Resource> resource;
        winrt::com_ptr<D3D12MA::Allocation> memoryAllocation;
        std::unordered_map<BufferUsage, winrt::com_ptr<DescriptorAllocation>> descriptorAllocations;
        size_t size;
        BufferUsageFlags flags;
    };

    class DX12Texture final : public Texture
    {
      public:
        DX12Texture(ID3D12Device1* device, D3D12MA::Allocator* memoryAllocator,
                    DescriptorAllocator* descriptorAllocator, TextureCreateInfo const& createInfo);

        /*!
            @brief Constructor for Swapchain Texture
        */
        DX12Texture(ID3D12Device1* device, DescriptorAllocator* descriptorAllocator,
                    winrt::com_ptr<ID3D12Resource> resource);

        auto getWidth() const -> uint32_t override;

        auto getHeight() const -> uint32_t override;

        auto getDepth() const -> uint32_t override;

        auto getMipLevels() const -> uint32_t override;

        auto getFormat() const -> TextureFormat override;

        auto getFlags() const -> TextureUsageFlags override;

        auto getDescriptorOffset(TextureUsage const usage) const -> uint32_t override;

        auto getResource() -> ID3D12Resource*;

        auto getDescriptor(TextureUsage const usage) const -> DescriptorAllocation*;

      private:
        D3D12MA::Allocator* memoryAllocator;
        DescriptorAllocator* descriptorAllocator;
        winrt::com_ptr<ID3D12Resource> resource;
        winrt::com_ptr<D3D12MA::Allocation> memoryAllocation;
        std::unordered_map<TextureUsage, winrt::com_ptr<DescriptorAllocation>> descriptorAllocations;
        uint32_t width;
        uint32_t height;
        uint32_t depth;
        uint32_t mipLevels;
        TextureFormat format;
        TextureDimension dimension;
        TextureUsageFlags flags;
    };

    enum class DX12ShaderStageType
    {
        Vertex,
        Pixel,
        Compute
    };

    class DX12VertexInput
    {
      public:
        DX12VertexInput(std::span<VertexDeclarationInfo const> const vertexDeclarations);

        auto getInputElementDescs() const -> std::span<D3D12_INPUT_ELEMENT_DESC const>;

        auto getInputSize() const -> uint32_t;

      private:
        std::vector<std::string> semanticNames;
        std::vector<D3D12_INPUT_ELEMENT_DESC> inputElementDescs;
        uint32_t inputSize;
    };

    struct DX12ShaderStage
    {
        std::vector<uint8_t> buffer;
        D3D12_SHADER_BYTECODE shaderByteCode;
    };

    class DX12Shader final : public Shader
    {
      public:
        DX12Shader(ID3D12Device4* device, ShaderCreateInfo const& createInfo);

        auto getHash() const -> uint64_t override;

        auto getPipelineType() const -> PipelineType override;

        auto getStages() const -> std::unordered_map<DX12ShaderStageType, DX12ShaderStage> const&;

        auto getVertexInput() const -> std::optional<DX12VertexInput>;

      private:
        std::optional<DX12VertexInput> vertexInput;
        std::unordered_map<DX12ShaderStageType, DX12ShaderStage> stages;
        uint64_t hash;
        PipelineType pipelineType;
    };

    class Pipeline final : public core::ref_counted_object
    {
      public:
        Pipeline(ID3D12Device4* device, ID3D12RootSignature* rootSignature, DX12Shader* shader,
                 RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                 std::optional<DepthStencilStageInfo> const depthStencil,
                 std::span<DXGI_FORMAT const> const renderTargetFormats, DXGI_FORMAT const depthStencilFormat,
                 ID3DBlob* blob);

        auto getPipelineState() -> ID3D12PipelineState*;

        auto getRootSignature() -> ID3D12RootSignature*;

      private:
        ID3D12RootSignature* rootSignature;
        winrt::com_ptr<ID3D12PipelineState> pipelineState;
    };

    class PipelineCache final : public core::ref_counted_object
    {
      public:
        struct Entry
        {
            uint64_t shaderHash;
            RasterizerStageInfo rasterizer;
            BlendColorInfo blendColor;
            std::optional<DepthStencilStageInfo> depthStencil;
            std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> renderTargetFormats;
            DXGI_FORMAT depthStencilFormat;

            auto operator==(Entry const& other) const -> bool
            {
                return std::make_tuple(shaderHash, rasterizer, blendColor,
                                       depthStencil.value_or(DepthStencilStageInfo::Default()), renderTargetFormats,
                                       depthStencilFormat) ==
                       std::make_tuple(other.shaderHash, other.rasterizer, other.blendColor,
                                       other.depthStencil.value_or(DepthStencilStageInfo::Default()),
                                       other.renderTargetFormats, other.depthStencilFormat);
            }
        };

        struct EntryHasher
        {
            auto operator()(const Entry& entry) const -> std::size_t
            {
                auto depthStencil = entry.depthStencil.value_or(DepthStencilStageInfo::Default());
                return entry.shaderHash ^ XXH64(&entry.rasterizer.fillMode, sizeof(uint32_t), 0) ^
                       XXH64(&entry.rasterizer.cullMode, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendDst, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendDstAlpha, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendEnable, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendOp, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendOpAlpha, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendSrc, sizeof(uint32_t), 0) ^
                       XXH64(&entry.blendColor.blendSrcAlpha, sizeof(uint32_t), 0) ^
                       XXH64(&depthStencil.depthFunc, sizeof(uint32_t), 0) ^
                       XXH64(&depthStencil.depthWrite, sizeof(uint32_t), 0) ^
                       XXH64(&depthStencil.stencilWrite, sizeof(uint32_t), 0) ^
                       XXH64(entry.renderTargetFormats.data(), entry.renderTargetFormats.size(), 0) ^
                       XXH64(&entry.depthStencilFormat, sizeof(DXGI_FORMAT), 0);
            }
        };

        PipelineCache(ID3D12Device4* device, RHICreateInfo const& createInfo);

        auto get(DX12Shader* shader, RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                 std::optional<DepthStencilStageInfo> const depthStencil,
                 std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> const& renderTargetFormats,
                 DXGI_FORMAT const depthStencilFormat) -> core::ref_ptr<Pipeline>;

        auto reset() -> void;

      private:
        std::mutex mutex;
        ID3D12Device4* device;
        winrt::com_ptr<ID3D12RootSignature> rootSignature;
        std::unordered_map<Entry, core::ref_ptr<Pipeline>, EntryHasher> entries;
    };

    class DX12Query final : public Query
    {
    };

    class DX12GraphicsContext final : public GraphicsContext
    {
      public:
        DX12GraphicsContext(ID3D12Device4* device, PipelineCache* pipelineCache,
                            DescriptorAllocator* descriptorAllocator, ID3D12CommandQueue* queue, ID3D12Fence* fence,
                            HANDLE fenceEvent, uint64_t& fenceValue);

        auto reset() -> void override;

        auto setGraphicsPipelineOptions(core::ref_ptr<Shader> shader, RasterizerStageInfo const& rasterizer,
                                        BlendColorInfo const& blendColor,
                                        std::optional<DepthStencilStageInfo> const depthStencil) -> void override;

        auto bindDescriptor(uint32_t const index, uint32_t const descriptor) -> void override;

        auto beginRenderPass(std::span<RenderPassColorInfo> const colors,
                             std::optional<RenderPassDepthStencilInfo> depthStencil) -> void override;

        auto endRenderPass() -> void override;

        auto bindVertexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size) -> void override;

        auto bindIndexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                             IndexFormat const format) -> void override;

        auto drawIndexed(uint32_t const indexCount, uint32_t const instanceCount) -> void override;

        auto draw(uint32_t const vertexCount, uint32_t const instanceCount) -> void override;

        auto setViewport(int32_t const x, int32_t const y, uint32_t const width,
                         uint32_t const height) -> void override;

        auto setScissor(int32_t const left, int32_t const top, int32_t const right,
                        int32_t const bottom) -> void override;

        auto barrier(core::ref_ptr<Buffer> dest, ResourceState const before,
                     ResourceState const after) -> void override;

        auto barrier(core::ref_ptr<Texture> dest, ResourceState const before,
                     ResourceState const after) -> void override;

        auto execute() -> Future<Query> override;

      private:
        ID3D12Device4* device;
        PipelineCache* pipelineCache;
        DescriptorAllocator* descriptorAllocator;
        ID3D12CommandQueue* queue;
        ID3D12Fence* fence;
        HANDLE fenceEvent;
        uint64_t* fenceValue;
        winrt::com_ptr<ID3D12CommandAllocator> commandAllocator;
        winrt::com_ptr<ID3D12GraphicsCommandList4> commandList;

        core::ref_ptr<DX12Shader> currentShader;
        bool isRootSignatureBinded;
        std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> renderTargetFormats;
        DXGI_FORMAT depthStencilFormat;
        std::array<uint32_t, 16> bindings;
    };

    class DX12CopyContext final : public CopyContext
    {
      public:
        DX12CopyContext(ID3D12Device4* device, D3D12MA::Allocator* memoryAllocator, ID3D12CommandQueue* queue,
                        ID3D12Fence* fence, HANDLE fenceEvent, uint64_t& fenceValue);

        auto reset() -> void override;

        auto writeBuffer(core::ref_ptr<Buffer> dest,
                         std::span<uint8_t const> const dataBytes) -> Future<Buffer> override;

        auto writeTexture(core::ref_ptr<Texture> dest, uint32_t const mipLevel,
                          std::span<uint8_t const> const dataBytes) -> Future<Texture> override;

        auto readBuffer(core::ref_ptr<Buffer> dest, uint8_t* dataBytes) -> size_t override;

        auto readTexture(core::ref_ptr<Texture> dest, uint32_t const mipLevel, uint8_t* dataBytes) -> size_t override;

        auto barrier(core::ref_ptr<Buffer> dest, ResourceState const before,
                     ResourceState const after) -> void override;

        auto barrier(core::ref_ptr<Texture> dest, ResourceState const before,
                     ResourceState const after) -> void override;

        auto execute() -> Future<Query> override;

      private:
        ID3D12Device4* device;
        ID3D12CommandQueue* queue;
        ID3D12Fence* fence;
        HANDLE fenceEvent;
        uint64_t* fenceValue;
        winrt::com_ptr<ID3D12CommandAllocator> commandAllocator;
        winrt::com_ptr<ID3D12GraphicsCommandList4> commandList;

        struct BufferInfo
        {
            core::ref_ptr<Buffer> buffer;
            uint64_t offset;
        };

        BufferInfo readStagingBuffer;
        BufferInfo writeStagingBuffer;

        auto getSurfaceData(rhi::TextureFormat const format, uint32_t const width, uint32_t const height,
                            size_t& rowBytes, uint32_t& rowCount) -> void;
    };

    class DX12FutureImpl final : public FutureImpl
    {
      public:
        DX12FutureImpl(ID3D12CommandQueue* queue, ID3D12Fence* fence, HANDLE fenceEvent, uint64_t const fenceValue);

        auto getResult() const -> bool override;

        auto wait() -> void override;

      private:
        ID3D12CommandQueue* queue;
        ID3D12Fence* fence;
        HANDLE fenceEvent;
        uint64_t fenceValue;
    };

    struct HANDLE_deleter
    {
        void operator()(HANDLE handle)
        {
            if (handle)
            {
                ::CloseHandle(handle);
            }
        }
    };

    using UniqueHandle = std::unique_ptr<HANDLE, HANDLE_deleter>;

    class DX12Device final : public Device
    {
      public:
        DX12Device(RHICreateInfo const& createInfo);

        auto createShader(ShaderCreateInfo const& createInfo) -> core::ref_ptr<Shader> override;

        auto createTexture(TextureCreateInfo const& createInfo) -> core::ref_ptr<Texture> override;

        auto createBuffer(BufferCreateInfo const& createInfo) -> core::ref_ptr<Buffer> override;

        auto createSampler(SamplerCreateInfo const& createInfo) -> core::ref_ptr<Sampler> override;

        auto createGraphicsContext() -> core::ref_ptr<GraphicsContext> override;

        auto createCopyContext() -> core::ref_ptr<CopyContext> override;

        auto requestBackBuffer() -> core::ref_ptr<Texture> override;

        auto presentBackBuffer() -> void override;

        auto resizeBackBuffers(uint32_t const width, uint32_t const height) -> void override;

        auto getBackendName() const -> std::string_view override;

      private:
#ifndef NDEBUG
        winrt::com_ptr<ID3D12Debug1> debug;
#endif
        winrt::com_ptr<IDXGIFactory4> factory;
        winrt::com_ptr<IDXGIAdapter1> adapter;
        winrt::com_ptr<ID3D12Device4> device;
        winrt::com_ptr<IDXGISwapChain3> swapchain;
        winrt::com_ptr<D3D12MA::Allocator> memoryAllocator;

        struct QueueInfo
        {
            winrt::com_ptr<ID3D12CommandQueue> queue;
            winrt::com_ptr<ID3D12Fence> fence;
            uint64_t fenceValue;
        };
        QueueInfo graphicsQueue;
        QueueInfo copyQueue;
        QueueInfo computeQueue;

        UniqueHandle fenceEvent;

        core::ref_ptr<DescriptorAllocator> descriptorAllocator;
        core::ref_ptr<PipelineCache> pipelineCache;

        std::vector<core::ref_ptr<Texture>> backBuffers;

        auto createSwapchainBuffers(uint32_t const width, uint32_t const height) -> void;
    };
} // namespace ionengine::rhi