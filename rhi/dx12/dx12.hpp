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
                        D3D12_DESCRIPTOR_HEAP_TYPE const heapType, uint32_t const incrementSize, uint32_t const offset)
            -> void;

        std::atomic<uint32_t> refCount;
    };

    enum class DescriptorAllocatorLimits : uint32_t
    {
        RTV = 128,
        CBV_SRV_UAV = 16 * 1024,
        Sampler = 128,
        DSV = 32
    };

    class DescriptorAllocator
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

        auto getSize() const -> size_t override;

        auto getFlags() const -> BufferUsageFlags override;

        auto mapMemory() -> uint8_t*;

        auto unmapMemory() -> void;

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

    class DX12Sampler final : public Sampler
    {
      public:
        DX12Sampler(ID3D12Device1* device, DescriptorAllocator* descriptorAllocator,
                    SamplerCreateInfo const& createInfo);

        auto getDescriptorOffset() const -> uint32_t override;

      private:
        winrt::com_ptr<DescriptorAllocation> descriptorAllocation;
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

        auto getInputElementDesc() const -> std::vector<D3D12_INPUT_ELEMENT_DESC> const&;

        auto getInputSize() const -> uint32_t;

      private:
        std::list<std::string> semanticNames;
        std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements;
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

        auto getShaderType() const -> ShaderType override;

        auto getHash() const -> uint64_t;

        auto getStages() const -> std::unordered_map<DX12ShaderStageType, DX12ShaderStage> const&;

        auto getVertexInput() const -> std::optional<DX12VertexInput> const&;

      private:
        std::optional<DX12VertexInput> vertexInput;
        std::unordered_map<DX12ShaderStageType, DX12ShaderStage> stages;
        uint64_t hash;
        ShaderType shaderType;
    };

    class Pipeline final : public core::ref_counted_object
    {
      public:
        Pipeline(ID3D12Device4* device, ID3D12RootSignature* rootSignature, DX12Shader* shader,
                 RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                 std::optional<DepthStencilStageInfo> const depthStencil,
                 std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> const& renderTargetFormats,
                 DXGI_FORMAT const depthStencilFormat, ID3DBlob* blob);

        auto getPipelineState() -> ID3D12PipelineState*;

        auto getRootSignature() -> ID3D12RootSignature*;

        auto getInputSize() const -> uint32_t;

      private:
        ID3D12RootSignature* rootSignature;
        winrt::com_ptr<ID3D12PipelineState> pipelineState;
        uint32_t inputSize;
    };

    class PipelineCache
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
            auto operator()(const Entry& other) const -> std::size_t
            {
                auto depthStencil = other.depthStencil.value_or(DepthStencilStageInfo::Default());
                return other.shaderHash ^ ::XXH64(&other.rasterizer.fillMode, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.rasterizer.cullMode, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendDst, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendDstAlpha, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendEnable, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendOp, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendOpAlpha, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendSrc, sizeof(uint32_t), 0) ^
                       ::XXH64(&other.blendColor.blendSrcAlpha, sizeof(uint32_t), 0) ^
                       ::XXH64(&depthStencil.depthFunc, sizeof(uint32_t), 0) ^
                       ::XXH64(&depthStencil.depthWrite, sizeof(uint32_t), 0) ^
                       ::XXH64(&depthStencil.stencilWrite, sizeof(uint32_t), 0) ^
                       ::XXH64(other.renderTargetFormats.data(), other.renderTargetFormats.size(), 0) ^
                       ::XXH64(&other.depthStencilFormat, sizeof(DXGI_FORMAT), 0);
            }
        };

        PipelineCache(ID3D12Device4* device, RHICreateInfo const& rhiCreateInfo);

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

    struct DeviceQueueData
    {
        winrt::com_ptr<ID3D12CommandQueue> queue;
        winrt::com_ptr<ID3D12Fence> fence;
        uint64_t fenceValue;
    };

    class DX12GraphicsContext final : public GraphicsContext
    {
      public:
        DX12GraphicsContext(ID3D12Device4* device, PipelineCache* pipelineCache,
                            DescriptorAllocator* descriptorAllocator, DeviceQueueData& deviceQueue, HANDLE fenceEvent,
                            uint32_t& curGraphicsContext);

        ~DX12GraphicsContext();

        auto setGraphicsPipelineOptions(core::ref_ptr<Shader> shader, RasterizerStageInfo const& rasterizer,
                                        BlendColorInfo const& blendColor,
                                        std::optional<DepthStencilStageInfo> const depthStencil) -> void override;

        auto bindDescriptor(uint32_t const index, uint32_t const descriptor) -> void override;

        auto beginRenderPass(std::span<RenderPassColorInfo const> const colors,
                             std::optional<RenderPassDepthStencilInfo> depthStencil) -> void override;

        auto endRenderPass() -> void override;

        auto bindVertexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size) -> void override;

        auto bindIndexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                             IndexFormat const format) -> void override;

        auto drawIndexed(uint32_t const indexCount, uint32_t const instanceCount) -> void override;

        auto draw(uint32_t const vertexCount, uint32_t const instanceCount) -> void override;

        auto setViewport(int32_t const x, int32_t const y, uint32_t const width, uint32_t const height)
            -> void override;

        auto setScissor(int32_t const left, int32_t const top, int32_t const right, int32_t const bottom)
            -> void override;

        auto barrier(core::ref_ptr<Buffer> dest, ResourceState const before, ResourceState const after)
            -> void override;

        auto barrier(core::ref_ptr<Texture> dest, ResourceState const before, ResourceState const after)
            -> void override;

        auto execute() -> Future<void> override;

        auto getCommandQueue() const -> ID3D12CommandQueue*;

      private:
        ID3D12Device4* device;
        PipelineCache* pipelineCache;
        DescriptorAllocator* descriptorAllocator;
        DeviceQueueData* deviceQueue;
        HANDLE fenceEvent;
        uint32_t* curGraphicsContext;
        winrt::com_ptr<ID3D12CommandAllocator> commandAllocator;
        winrt::com_ptr<ID3D12GraphicsCommandList4> commandList;
        bool isCommandListOpened;
        core::ref_ptr<Pipeline> currentPipeline;
        std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> renderTargetFormats;
        DXGI_FORMAT depthStencilFormat;
        std::vector<uint8_t> bindingData;

        auto tryResetCommandList() -> void;
    };

    class DX12CopyContext final : public CopyContext
    {
      public:
        DX12CopyContext(ID3D12Device4* device, D3D12MA::Allocator* memoryAllocator, DeviceQueueData& deviceQueue,
                        HANDLE fenceEvent, RHICreateInfo const& rhiCreateInfo, uint32_t& curCopyContext);

        ~DX12CopyContext();

        auto updateBuffer(core::ref_ptr<Buffer> dest, uint64_t const offset, std::span<uint8_t const> const dataBytes)
            -> Future<Buffer> override;

        auto updateTexture(core::ref_ptr<Texture> dest, uint32_t const resourceIndex,
                           std::span<uint8_t const> const dataBytes) -> Future<Texture> override;

        auto barrier(core::ref_ptr<Buffer> dest, ResourceState const before, ResourceState const after)
            -> void override;

        auto barrier(core::ref_ptr<Texture> dest, ResourceState const before, ResourceState const after)
            -> void override;

        auto execute() -> Future<void> override;

        auto getCommandQueue() const -> ID3D12CommandQueue*;

      private:
        ID3D12Device4* device;
        DeviceQueueData* deviceQueue;
        HANDLE fenceEvent;
        uint32_t* curCopyContext;
        winrt::com_ptr<ID3D12CommandAllocator> commandAllocator;
        winrt::com_ptr<ID3D12GraphicsCommandList4> commandList;
        bool isCommandListOpened;

        struct StagingBufferData
        {
            core::ref_ptr<DX12Buffer> buffer;
            uint64_t offset;
        };

        StagingBufferData readStagingBuffer;
        StagingBufferData writeStagingBuffer;

        auto getSurfaceData(DXGI_FORMAT const format, uint32_t const width, uint32_t const height, size_t& rowBytes,
                            uint32_t& rowCount) -> void;

        auto tryResetCommandList() -> void;
    };

    class DX12FutureImpl final : public FutureImpl
    {
      public:
        DX12FutureImpl(ID3D12CommandQueue* queue, ID3D12Fence* fence, HANDLE fenceEvent, uint64_t const fenceValue);

        auto getResult() const -> bool override;

        auto wait() -> void override;

        auto waitOnContext(IDeviceContext* context) -> void override;

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

    using UniqueHandle = std::unique_ptr<std::remove_pointer<HANDLE>::type, HANDLE_deleter>;

    class DX12Swapchain : public Swapchain
    {
      public:
        DX12Swapchain(IDXGIFactory4* factory, ID3D12Device4* device, DescriptorAllocator* descriptorAllocator,
                      DeviceQueueData& deviceQueue, HANDLE fenceEvent, SwapchainCreateInfo const& createInfo);

        ~DX12Swapchain();

        auto getBackBuffer() -> core::weak_ptr<Texture> override;

        auto presentBackBuffer() -> void override;

        auto resizeBackBuffers(uint32_t const width, uint32_t const height) -> void override;

      private:
        ID3D12Device4* device;
        DescriptorAllocator* descriptorAllocator;
        DeviceQueueData* deviceQueue;
        HANDLE fenceEvent;
        winrt::com_ptr<IDXGISwapChain3> swapchain;
        std::vector<core::ref_ptr<Texture>> backBuffers;

        auto createSwapchainBuffers(uint32_t const width, uint32_t const height) -> void;
    };

    class DX12RHI final : public RHI
    {
      public:
        DX12RHI(RHICreateInfo const& rhiCreateInfo, std::optional<SwapchainCreateInfo> const swapchainCreateInfo);

        auto createShader(ShaderCreateInfo const& createInfo) -> core::ref_ptr<Shader> override;

        auto createTexture(TextureCreateInfo const& createInfo) -> core::ref_ptr<Texture> override;

        auto createBuffer(BufferCreateInfo const& createInfo) -> core::ref_ptr<Buffer> override;

        auto createSampler(SamplerCreateInfo const& createInfo) -> core::ref_ptr<Sampler> override;

        auto getSwapchain() -> Swapchain* override;

        auto getGraphicsContext() -> GraphicsContext* override;

        auto getCopyContext() -> CopyContext* override;

        auto getName() const -> std::string const& override;

      private:
        std::string const rhiName{"D3D12"};

#ifndef NDEBUG
        winrt::com_ptr<ID3D12Debug1> debug;
#endif
        winrt::com_ptr<IDXGIFactory4> factory;
        winrt::com_ptr<IDXGIAdapter1> adapter;
        winrt::com_ptr<ID3D12Device4> device;
        winrt::com_ptr<D3D12MA::Allocator> memoryAllocator;
        UniqueHandle fenceEvent;

        DeviceQueueData graphicsQueue;
        DeviceQueueData copyQueue;
        DeviceQueueData computeQueue;

        std::unique_ptr<DescriptorAllocator> descriptorAllocator;
        std::unique_ptr<PipelineCache> pipelineCache;

        struct FrameContextData
        {
            std::unique_ptr<DX12GraphicsContext> graphicsContext;
            std::unique_ptr<DX12CopyContext> copyContext;
        };
        std::vector<FrameContextData> frameContexts;

        uint32_t curGraphicsContext;
        uint32_t curCopyContext;

        std::unique_ptr<DX12Swapchain> swapchain;

        auto createDeviceQueue(D3D12_COMMAND_LIST_TYPE const commandListType, DeviceQueueData& deviceQueue) -> void;
    };
} // namespace ionengine::rhi