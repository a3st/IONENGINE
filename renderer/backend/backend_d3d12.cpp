// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/backend/backend.h>
#include <renderer/backend/d3d12x.h>
#include <platform/window.h>
#include <lib/exception.h>
#include <lib/algorithm.h>
#include <d3d12_ma/D3D12MemAlloc.h>

using namespace ionengine;
using namespace ionengine::renderer;
using namespace ionengine::renderer::backend;

namespace ionengine::renderer::backend {

struct Texture {
    ComPtr<ID3D12Resource> resource;
    ComPtr<D3D12MA::Allocation> memory_allocation;
    std::unordered_map<D3D12_DESCRIPTOR_HEAP_TYPE, ComPtr<d3d12::DescriptorAllocation>> descriptor_allocations;
};

struct Buffer {
    ComPtr<ID3D12Resource> resource;
    ComPtr<D3D12MA::Allocation> memory_allocation;
    ComPtr<d3d12::DescriptorAllocation> descriptor_allocation;
};

struct Sampler {
    ComPtr<d3d12::DescriptorAllocation> descriptor_allocation;
};

struct BindingLocation {
    uint32_t range_index;
    uint32_t offset;
};

struct DescriptorLayout {
    ComPtr<ID3D12RootSignature> root_signature;
    std::unordered_map<uint32_t, BindingLocation> bindings;
    std::vector<D3D12_DESCRIPTOR_RANGE> ranges;
    bool is_compute;
};

struct Pipeline {
    ComPtr<ID3D12RootSignature> root_signature;
    ComPtr<ID3D12PipelineState> pipeline_state;
    std::array<uint32_t, 16> vertex_strides;
    bool is_compute;
};

struct Shader {
    std::vector<char8_t> data;
    ShaderFlags flags;
};

struct RenderPass {
    std::array<D3D12_RENDER_PASS_RENDER_TARGET_DESC, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> render_target_descs;
    uint32_t render_target_count;
    D3D12_RENDER_PASS_DEPTH_STENCIL_DESC depth_stencil_desc;
    bool has_depth_stencil;
};

struct CommandList {
    ComPtr<ID3D12CommandAllocator> command_allocator;
    ComPtr<ID3D12GraphicsCommandList4> command_list;
    Pipeline* binded_pipeline;
    QueueFlags flags;
    bool is_reset{false};
};

}

struct Device::Impl {

    ComPtr<IDxcCompiler3> dxc_compiler;
    ComPtr<IDxcUtils> dxc_utils;

    ComPtr<D3D12MA::Allocator> memory_allocator;

    ComPtr<d3d12::DescriptorPool> cbv_srv_uav_pool;
    ComPtr<d3d12::DescriptorPool> rtv_pool;
    ComPtr<d3d12::DescriptorPool> dsv_pool;
    ComPtr<d3d12::DescriptorPool> sampler_pool;

    ComPtr<ID3D12DescriptorHeap> cbv_srv_uav_heap;
    std::vector<ComPtr<d3d12::DescriptorRange>> cbv_srv_uav_ranges;

    ComPtr<ID3D12DescriptorHeap> sampler_heap;
    std::vector<ComPtr<d3d12::DescriptorRange>> sampler_ranges;

#ifdef _DEBUG
    ComPtr<ID3D12Debug> debug;
    ComPtr<ID3D12InfoQueue> info_queue;
    ComPtr<ID3D12DeviceRemovedExtendedDataSettings1> dred_settings;
#endif

    ComPtr<IDXGIFactory4> factory;
    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<ID3D12Device4> device;
    ComPtr<IDXGISwapChain3> swapchain;
    std::vector<ComPtr<ID3D12CommandQueue>> queues;
    std::vector<ComPtr<ID3D12Fence>> fences;
    std::array<std::atomic<uint64_t>, 3> fence_values;
    std::vector<ID3D12CommandList*> batches;

    HANDLE fence_event{NULL};

    HandlePool<Texture> textures;
    HandlePool<Buffer> buffers;
    HandlePool<Sampler> samplers;
    HandlePool<DescriptorLayout> descriptor_layouts;
    HandlePool<Pipeline> pipelines;
    HandlePool<Shader> shaders;
    HandlePool<RenderPass> render_passes;
    HandlePool<CommandList> command_lists;

    uint32_t swapchain_index{0};
    std::vector<Handle<Texture>> swapchain_textures;

    Impl(
        uint32_t const texture_size, 
        uint32_t const buffer_size, 
        uint32_t const sampler_size, 
        uint32_t const descriptor_layout_size, 
        uint32_t const pipeline_size,
        uint32_t const shader_size,
        uint32_t const render_pass_size,
        uint32_t const command_list_size
    );

    void initialize(uint32_t const adapter_index, SwapchainDesc const& _swapchain_desc);

    void deinitialize();

    void create_swapchain(SwapchainDesc const& _swapchain_desc);

    void create_swapchain_resources();

    void command_list_reset(CommandList& command_list);
    
    Handle<Texture> create_texture(
        Dimension const dimension, 
        uint32_t const width, 
        uint32_t const height, 
        uint16_t const mip_levels, 
        uint16_t const array_layers, 
        Format const format, 
        TextureFlags const flags
    );
    
    void delete_texture(Handle<Texture> const& texture);
    
    Handle<Buffer> create_buffer(size_t const size, BufferFlags const flags, uint32_t const element_stride = 0);
    
    void delete_buffer(Handle<Buffer> const& buffer);
    
    Handle<RenderPass> create_render_pass(
        std::span<Handle<Texture> const> const& colors, 
        std::span<RenderPassColorDesc const> const& color_descs, 
        Handle<Texture> const& depth_stencil = InvalidHandle<Texture>(),
        std::optional<RenderPassDepthStencilDesc> const depth_stencil_desc = std::nullopt
    );
    
    void delete_render_pass(Handle<RenderPass> const& render_pass);
    
    Handle<Sampler> create_sampler(
        Filter const filter, 
        AddressMode const address_u, 
        AddressMode const address_v, 
        AddressMode const address_w, 
        uint16_t const anisotropic, 
        CompareOp const compare_op
    );
    
    void delete_sampler(Handle<Sampler> const& sampler);

    Handle<Shader> create_shader(std::string_view const source, ShaderFlags const flags);
    
    void delete_shader(Handle<Shader> const& shader);
    
    Handle<DescriptorLayout> create_descriptor_layout(std::span<DescriptorLayoutBinding const> const bindings, bool const is_compute);
    
    void delete_descriptor_layout(Handle<DescriptorLayout> const& descriptor_layout);
    
    Handle<Pipeline> create_pipeline(
        Handle<DescriptorLayout> const& descriptor_layout, 
        std::span<VertexInputDesc const> const vertex_descs, 
        std::span<Handle<Shader> const> const shaders, 
        RasterizerDesc const& rasterizer_desc, 
        DepthStencilDesc const& depth_stencil_desc, 
        BlendDesc const& blend_desc, 
        Handle<RenderPass> const& render_pass,
        Handle<CachePipeline> const& cache_pipeline
    );

    Handle<Pipeline> create_pipeline(
        Handle<DescriptorLayout> const& descriptor_layout,
        Handle<Shader> const& shader,
        Handle<CachePipeline> const& cache_pipeline
    );
    
    void delete_pipeline(Handle<Pipeline> const& pipeline);

    Handle<CommandList> create_command_list(QueueFlags const flags, bool const bundle = false);

    void delete_command_list(Handle<CommandList> const& command_list);

    uint8_t* map_buffer_data(Handle<Buffer> const& buffer, uint64_t const offset = 0);

    void unmap_buffer_data(Handle<Buffer> const& buffer);
    
    void present();

    size_t get_memory_required_size(ResourceHandle const& resource);

    uint32_t acquire_next_swapchain_texture();

    Handle<Texture> swapchain_texture(uint32_t const index);

    void recreate_swapchain(uint32_t const width, uint32_t const height, std::optional<SwapchainDesc> swapchain_desc = std::nullopt);

    uint64_t submit(std::span<Handle<CommandList> const> const command_lists, QueueFlags const flags);

    uint64_t submit_after(std::span<Handle<CommandList> const> const command_lists, uint64_t const fence_value, QueueFlags const flags);

    void wait(uint64_t const fence_value, QueueFlags const flags);

    bool is_completed(uint64_t const fence_value, QueueFlags const flags) const;

    void wait_for_idle(QueueFlags const flags);

    void copy_buffer_region(
        Handle<CommandList> const& command_list,
        Handle<Buffer> const& dest, 
        uint64_t const dest_offset, 
        Handle<Buffer> const& source, 
        uint64_t const source_offset,
        size_t const size
    );

    void copy_texture_region(
        Handle<CommandList> const& command_list,
        Handle<Texture> const& dest,
        Handle<Buffer> const& source,
        std::span<TextureCopyRegion const> const regions
    );

    void bind_vertex_buffer(Handle<CommandList> const& command_list, uint32_t const index, Handle<Buffer> const& buffer, uint64_t const offset);

    void bind_index_buffer(Handle<CommandList> const& command_list, Handle<Buffer> const& buffer, uint64_t const offset);

    void barrier(Handle<CommandList> const& command_list, std::span<MemoryBarrierDesc const> const barriers);

    void bind_pipeline(Handle<CommandList> const& command_list, Handle<Pipeline> const& pipeline);
    
    void bind_resources(Handle<CommandList> const& command_list, Handle<DescriptorLayout> const& descriptor_layout, std::span<DescriptorWriteDesc const> const write_descs);

    void set_viewport(Handle<CommandList> const& command_list, int32_t const x, int32_t const y, uint32_t const width, uint32_t const height);

    void set_scissor(Handle<CommandList> const& command_list, int32_t const left, int32_t const top, int32_t const right, int32_t const bottom);

    void begin_render_pass(
        Handle<CommandList> const& command_list,
        Handle<RenderPass> const& render_pass, 
        std::span<lib::math::Color const> const clear_colors, 
        float const clear_depth = 0.0f,
        uint8_t const clear_stencil = 0x0
    );

    void end_render_pass(Handle<CommandList> const& command_list);

    void begin_command_list(Handle<CommandList> const& command_list);

    void close_command_list(Handle<CommandList> const& command_list);

    void execute_bundle(Handle<CommandList> const& command_list, Handle<CommandList> const& bundle_command_list);

    void dispatch(Handle<CommandList> const& command_list, uint32_t const thread_group_x, uint32_t const thread_group_y, uint32_t const thread_group_z);

    void draw(Handle<CommandList> const& command_list, uint32_t const vertex_count, uint32_t const instance_count, uint32_t const vertex_offset);

    void draw_indexed(Handle<CommandList> const& command_list, uint32_t const index_count, uint32_t const instance_count, uint32_t const instance_offset);

    AdapterDesc adapter_desc() const;
};

void Device::impl_deleter::operator()(Impl* ptr) const {

    delete ptr;
}

//===========================================================
//
//
//          Backend Implementation
//
//
//===========================================================

Device::Impl::Impl(
    uint32_t const texture_size, 
    uint32_t const buffer_size, 
    uint32_t const sampler_size, 
    uint32_t const descriptor_layout_size, 
    uint32_t const pipeline_size,
    uint32_t const shader_size,
    uint32_t const render_pass_size,
    uint32_t const command_list_size
) : 
    textures(texture_size),
    buffers(buffer_size),
    samplers(sampler_size),
    descriptor_layouts(descriptor_layout_size),
    pipelines(pipeline_size),
    shaders(shader_size),
    render_passes(render_pass_size),
    command_lists(command_list_size) {

}

void Device::Impl::initialize(uint32_t const adapter_index, SwapchainDesc const& _swapchain_desc) {

    THROW_IF_FAILED(DxcCreateInstance(CLSID_DxcCompiler, __uuidof(IDxcCompiler3), reinterpret_cast<void**>(dxc_compiler.GetAddressOf())));
    THROW_IF_FAILED(DxcCreateInstance(CLSID_DxcUtils, __uuidof(IDxcUtils), reinterpret_cast<void**>(dxc_utils.GetAddressOf())));

    uint32_t factory_flags = 0;

#ifdef _DEBUG
    // Create Debug Layer. Only in debug engine build
    {
        THROW_IF_FAILED(D3D12GetDebugInterface(__uuidof(ID3D12Debug), reinterpret_cast<void**>(debug.GetAddressOf())));
        debug->EnableDebugLayer();

        THROW_IF_FAILED(D3D12GetDebugInterface(IID_PPV_ARGS(&dred_settings)));
        dred_settings->SetAutoBreadcrumbsEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);
        dred_settings->SetPageFaultEnablement(D3D12_DRED_ENABLEMENT_FORCED_ON);

        // Factory debug flags
        factory_flags = DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    THROW_IF_FAILED(CreateDXGIFactory2(factory_flags, __uuidof(IDXGIFactory4), reinterpret_cast<void**>(factory.GetAddressOf())));
    THROW_IF_FAILED(factory->EnumAdapters1(adapter_index, adapter.GetAddressOf()));
    THROW_IF_FAILED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device4), reinterpret_cast<void**>(device.GetAddressOf())));

#ifdef _DEBUG
    // Setup Info Queue. Only in debug engine build 
    {
        THROW_IF_FAILED(device->QueryInterface(info_queue.GetAddressOf()));

        auto info_queue_filter = D3D12_INFO_QUEUE_FILTER {};
        
        std::array<D3D12_MESSAGE_SEVERITY, 2> allow_severities = {
            D3D12_MESSAGE_SEVERITY_ERROR,
            D3D12_MESSAGE_SEVERITY_WARNING
        };

        info_queue_filter.AllowList.pSeverityList = allow_severities.data();
        info_queue_filter.AllowList.NumSeverities = static_cast<uint32_t>(allow_severities.size());
        info_queue->PushStorageFilter(&info_queue_filter);
    }
#endif
     
    // Initialize CBV_SRV_UAV Heap Pool for resources
    THROW_IF_FAILED(d3d12::create_descriptor_pool(
        device.Get(), 
        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        8096, 
        cbv_srv_uav_pool.GetAddressOf())
    );

    // Initialize RTV Heap Pool for resources
    THROW_IF_FAILED(d3d12::create_descriptor_pool(
        device.Get(), 
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        256, 
        rtv_pool.GetAddressOf())
    );
    
    // Initialize DSV Heap Pool for resources
    THROW_IF_FAILED(d3d12::create_descriptor_pool(
        device.Get(), 
        D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        12, 
        dsv_pool.GetAddressOf())
    );
    
    // Initialize Sampler Heap Pool for resources
    THROW_IF_FAILED(d3d12::create_descriptor_pool(
        device.Get(), 
        D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 
        D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
        2048, 
        sampler_pool.GetAddressOf())
    );

    // Create CBV_SRV_UAV Heap for binding in Descriptor Table
    {
        auto descriptor_heap_desc = D3D12_DESCRIPTOR_HEAP_DESC {};
        descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        descriptor_heap_desc.NumDescriptors = 4096;
        descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        THROW_IF_FAILED(device->CreateDescriptorHeap(&descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(cbv_srv_uav_heap.GetAddressOf())));
    }

    // Create Sampler Heap for binding in Descriptor Table
    {
        auto descriptor_heap_desc = D3D12_DESCRIPTOR_HEAP_DESC {};
        descriptor_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        descriptor_heap_desc.NumDescriptors = 2048;
        descriptor_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

        THROW_IF_FAILED(device->CreateDescriptorHeap(&descriptor_heap_desc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(sampler_heap.GetAddressOf())));
    }

    // Create Memory Allocator
    {
        auto allocator_desc = D3D12MA::ALLOCATOR_DESC {};
        allocator_desc.pDevice = device.Get();
        allocator_desc.pAdapter = adapter.Get();
 
        THROW_IF_FAILED(D3D12MA::CreateAllocator(&allocator_desc, memory_allocator.GetAddressOf()));
    }

    queues.resize(3);
    fences.resize(3);
    std::fill(fence_values.begin(), fence_values.end(), 1);

    // Create Device Queue (Direct) for execution command lists
    // Create Fence Object (Direct) for synchronization
    {
        auto command_queue_desc = D3D12_COMMAND_QUEUE_DESC {};
        command_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

        THROW_IF_FAILED(device->CreateCommandQueue(
            &command_queue_desc,
            __uuidof(ID3D12CommandQueue), 
            reinterpret_cast<void**>(queues[0].GetAddressOf()))
        );

        THROW_IF_FAILED(device->CreateFence(
            0, 
            D3D12_FENCE_FLAG_NONE, 
            __uuidof(ID3D12Fence), 
            reinterpret_cast<void**>(fences[0].GetAddressOf()))
        );
    }

    // Create Device Queue (Copy) for execution command lists
    // Create Fence Object (Copy) for synchronization
    {
        auto command_queue_desc = D3D12_COMMAND_QUEUE_DESC {};
        command_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

        THROW_IF_FAILED(device->CreateCommandQueue(
            &command_queue_desc,
            __uuidof(ID3D12CommandQueue), 
            reinterpret_cast<void**>(queues[1].GetAddressOf()))
        );

        THROW_IF_FAILED(device->CreateFence(
            0, 
            D3D12_FENCE_FLAG_NONE, 
            __uuidof(ID3D12Fence), 
            reinterpret_cast<void**>(fences[1].GetAddressOf()))
        );
    }

    // Create Device Queue (Compute) for execution command lists
    // Create Fence Object (Compute) for synchronization
    {
        auto command_queue_desc = D3D12_COMMAND_QUEUE_DESC {};
        command_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

        THROW_IF_FAILED(device->CreateCommandQueue(
            &command_queue_desc,
            __uuidof(ID3D12CommandQueue), 
            reinterpret_cast<void**>(queues[2].GetAddressOf()))
        );

        THROW_IF_FAILED(device->CreateFence(
            0, 
            D3D12_FENCE_FLAG_NONE, 
            __uuidof(ID3D12Fence), 
            reinterpret_cast<void**>(fences[2].GetAddressOf()))
        );
    }

    // Create WINAPI Fence Object for synchronization
    {
        fence_event = CreateEvent(nullptr, false, false, nullptr);
        if(!fence_event) {
            THROW_IF_FAILED(HRESULT_FROM_WIN32(GetLastError()));
        }
    }

    // Create Swapchain using Device Queue (Direct)
    {
        auto swapchain_desc = DXGI_SWAP_CHAIN_DESC1 {};
        swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapchain_desc.Width = _swapchain_desc.window->client_width();
        swapchain_desc.Height = _swapchain_desc.window->client_height();
        swapchain_desc.BufferCount = _swapchain_desc.buffer_count;
        swapchain_desc.SampleDesc.Count = _swapchain_desc.sample_count;
        swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapchain_desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

        THROW_IF_FAILED(factory->CreateSwapChainForHwnd(
            queues[0].Get(), 
            reinterpret_cast<HWND>(_swapchain_desc.window->native_handle()), 
            &swapchain_desc, 
            nullptr, 
            nullptr, 
            reinterpret_cast<IDXGISwapChain1**>(swapchain.GetAddressOf()))
        );
    }

    swapchain_textures.resize(_swapchain_desc.buffer_count);
    cbv_srv_uav_ranges.resize(_swapchain_desc.buffer_count);
    sampler_ranges.resize(_swapchain_desc.buffer_count);

    // Get Swapchain resources and create RTV's
    {
        auto swapchain_desc = DXGI_SWAP_CHAIN_DESC1 {};
        swapchain->GetDesc1(&swapchain_desc);

        for(uint32_t i = 0; i < _swapchain_desc.buffer_count; ++i) {

            auto texture = Texture {};

            THROW_IF_FAILED(swapchain->GetBuffer(i, __uuidof(ID3D12Resource), reinterpret_cast<void**>(texture.resource.GetAddressOf())));

            auto render_target_view_desc = D3D12_RENDER_TARGET_VIEW_DESC {};
            render_target_view_desc.Format = swapchain_desc.Format;
            render_target_view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

            auto& cur_allocation = texture.descriptor_allocations[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
            THROW_IF_FAILED(rtv_pool->allocate(cur_allocation.GetAddressOf()));

            device->CreateRenderTargetView(texture.resource.Get(), &render_target_view_desc, cur_allocation->cpu_handle());

            swapchain_textures[i] = textures.push(std::move(texture));

            THROW_IF_FAILED(d3d12::create_descriptor_range(device.Get(), cbv_srv_uav_heap.Get(), i * 1024, 1024, cbv_srv_uav_ranges[i].GetAddressOf()));
            THROW_IF_FAILED(d3d12::create_descriptor_range(device.Get(), sampler_heap.Get(), i * 1024, 1024, sampler_ranges[i].GetAddressOf()));
        }
    }
}

void Device::Impl::deinitialize() {
    wait_for_idle(QueueFlags::Graphics | QueueFlags::Copy | QueueFlags::Compute);
    CloseHandle(fence_event);
}

Handle<Texture> Device::Impl::create_texture(
    Dimension const dimension,
    uint32_t const width,
    uint32_t const height,
    uint16_t const mip_levels,
    uint16_t const array_layers,
    Format const format,
    TextureFlags const flags
) {

    auto get_dimension_type = [&](Dimension const dimension) -> D3D12_RESOURCE_DIMENSION {
        switch(dimension) {
            case Dimension::_1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
            case Dimension::Cube:
            case Dimension::_2D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            case Dimension::_3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
        }
        return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    };

    auto get_format = [&](Format const format) -> DXGI_FORMAT {
        switch(format) {
            case Format::Unknown: return DXGI_FORMAT_UNKNOWN;
            case Format::RGBA8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
            case Format::BGRA8_UNORM: return DXGI_FORMAT_B8G8R8A8_UNORM;
            case Format::BGR8_UNORM: return DXGI_FORMAT_B8G8R8X8_UNORM;
            case Format::R8_UNORM: return DXGI_FORMAT_R8_UNORM;
            case Format::BC1: return DXGI_FORMAT_BC1_UNORM;
            case Format::BC3: return DXGI_FORMAT_BC3_UNORM;
            case Format::BC4: return DXGI_FORMAT_BC4_UNORM;
            case Format::BC5: return DXGI_FORMAT_BC5_UNORM;
            case Format::D32_FLOAT: return DXGI_FORMAT_R32_TYPELESS;
            case Format::RGBA16_FLOAT: return DXGI_FORMAT_R16G16B16A16_FLOAT;
            default: assert(false && "invalid format specified"); break;
        }
        return DXGI_FORMAT_UNKNOWN;
    };

    auto texture_data = Texture {};

    auto resource_desc = D3D12_RESOURCE_DESC {};
    resource_desc.Dimension = get_dimension_type(dimension);
    resource_desc.Width = width;
    resource_desc.Height = height;
    resource_desc.MipLevels = mip_levels;
    resource_desc.DepthOrArraySize = array_layers;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.Format = get_format(format);
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_RESOURCE_STATES initial_state = D3D12_RESOURCE_STATE_COMMON;

    if(flags & TextureFlags::DepthStencil) {
        resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
        initial_state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    }
    if(flags & TextureFlags::RenderTarget) {
        resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        initial_state = D3D12_RESOURCE_STATE_RENDER_TARGET;
    }
    if(flags & TextureFlags::UnorderedAccess) {
        resource_desc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
    }

    auto allocation_desc = D3D12MA::ALLOCATION_DESC {};
    if(flags & TextureFlags::HostWrite) {
        allocation_desc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
        initial_state = D3D12_RESOURCE_STATE_GENERIC_READ;
    } else {
        allocation_desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
    }
    
    THROW_IF_FAILED(memory_allocator->CreateResource(
        &allocation_desc,
        &resource_desc,
        initial_state,
        nullptr,
        texture_data.memory_allocation.GetAddressOf(),
        __uuidof(ID3D12Resource), 
        reinterpret_cast<void**>(texture_data.resource.GetAddressOf()))
    );

    if(flags & TextureFlags::DepthStencil) {

        auto depth_stencil_view_desc = D3D12_DEPTH_STENCIL_VIEW_DESC {};
        depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;

        switch(dimension) {
            case Dimension::_1D: {
                depth_stencil_view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE1D;
                depth_stencil_view_desc.Texture1D.MipSlice = mip_levels;
            } break;
            case Dimension::_2D: {
                depth_stencil_view_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                depth_stencil_view_desc.Texture2D.MipSlice = mip_levels - 1;
            } break;
            default: {
                assert(false && "depth stencil view dimension is unsupported");
            } break;
        }

        auto& cur_allocation = texture_data.descriptor_allocations[D3D12_DESCRIPTOR_HEAP_TYPE_DSV];
        THROW_IF_FAILED(dsv_pool->allocate(cur_allocation.GetAddressOf()));

        device->CreateDepthStencilView(texture_data.resource.Get(), &depth_stencil_view_desc, cur_allocation->cpu_handle());
    }
    if(flags & TextureFlags::RenderTarget) {

        auto render_target_view_desc = D3D12_RENDER_TARGET_VIEW_DESC {};
        render_target_view_desc.Format = resource_desc.Format;

        switch(dimension) {
            case Dimension::_1D: {
                render_target_view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE1D;
                render_target_view_desc.Texture1D.MipSlice = mip_levels;
            } break;
            case Dimension::_2D: {
                render_target_view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
                render_target_view_desc.Texture2D.MipSlice = mip_levels - 1;
            } break;
            case Dimension::_3D: {
                render_target_view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE3D;
                render_target_view_desc.Texture3D.MipSlice = mip_levels;
                render_target_view_desc.Texture3D.WSize = array_layers;
            } break;
            case Dimension::Cube: {
                render_target_view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DARRAY;
                render_target_view_desc.Texture2DArray.MipSlice = mip_levels - 1;
                render_target_view_desc.Texture2DArray.ArraySize = 6;
            } break;
        }

        auto& cur_allocation = texture_data.descriptor_allocations[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
        THROW_IF_FAILED(rtv_pool->allocate(cur_allocation.GetAddressOf()));

        device->CreateRenderTargetView(texture_data.resource.Get(), &render_target_view_desc, cur_allocation->cpu_handle());
    }
    if(flags & TextureFlags::UnorderedAccess) {

        auto unordered_access_view_desc = D3D12_UNORDERED_ACCESS_VIEW_DESC {};
        unordered_access_view_desc.Format = resource_desc.Format;
        
        switch(dimension) {
            case Dimension::_1D: {
                unordered_access_view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE1D;
                unordered_access_view_desc.Texture1D.MipSlice = mip_levels;
            } break;
            case Dimension::_2D: {
                unordered_access_view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
                unordered_access_view_desc.Texture2D.MipSlice = mip_levels - 1;
            } break;
            case Dimension::_3D: {
                unordered_access_view_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE3D;
                unordered_access_view_desc.Texture3D.MipSlice = mip_levels;
                unordered_access_view_desc.Texture3D.WSize = array_layers;
            } break;
        }

        auto& cur_allocation = texture_data.descriptor_allocations[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
        THROW_IF_FAILED(cbv_srv_uav_pool->allocate(cur_allocation.GetAddressOf()));

        device->CreateUnorderedAccessView(texture_data.resource.Get(), nullptr, &unordered_access_view_desc, cur_allocation->cpu_handle());
    }
    if(flags & TextureFlags::ShaderResource) {

        auto shader_resource_view_desc = D3D12_SHADER_RESOURCE_VIEW_DESC {};
        if(flags & TextureFlags::DepthStencil) {
            shader_resource_view_desc.Format = DXGI_FORMAT_R32_FLOAT;
        } else {
            shader_resource_view_desc.Format = resource_desc.Format;
        }
        shader_resource_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        switch(dimension) {
            case Dimension::_1D: {
                shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
                shader_resource_view_desc.Texture1D.MipLevels = mip_levels;
            } break;
            case Dimension::_2D: {
                shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                shader_resource_view_desc.Texture2D.MipLevels = mip_levels;
            } break;
            case Dimension::_3D: {
                shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
                shader_resource_view_desc.Texture3D.MipLevels = mip_levels;
            } break;
            case Dimension::Cube: {
                shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                shader_resource_view_desc.Texture3D.MipLevels = mip_levels;
            } break;
        }

        auto& cur_allocation = texture_data.descriptor_allocations[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
        THROW_IF_FAILED(cbv_srv_uav_pool->allocate(cur_allocation.GetAddressOf()));

        device->CreateShaderResourceView(texture_data.resource.Get(), &shader_resource_view_desc, cur_allocation->cpu_handle());
    }

    return textures.push(std::move(texture_data));
}

void Device::Impl::delete_texture(Handle<Texture> const& texture) {

    textures.erase(texture);
}

Handle<Buffer> Device::Impl::create_buffer(size_t const size, BufferFlags const flags, uint32_t const element_stride) {

    auto buffer_data = Buffer {};

    auto resource_desc = D3D12_RESOURCE_DESC {};
    resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resource_desc.Width = size;
    resource_desc.Height = 1;
    resource_desc.MipLevels = 1;
    resource_desc.DepthOrArraySize = 1;
    resource_desc.SampleDesc.Count = 1;
    resource_desc.Format = DXGI_FORMAT_UNKNOWN;
    resource_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    D3D12_RESOURCE_STATES initial_state;
    auto allocation_desc = D3D12MA::ALLOCATION_DESC {};

    if(flags & BufferFlags::HostWrite) {
        initial_state = D3D12_RESOURCE_STATE_GENERIC_READ;
        allocation_desc.HeapType = D3D12_HEAP_TYPE_UPLOAD;
    } else if(flags & BufferFlags::HostRead) {
        initial_state = D3D12_RESOURCE_STATE_GENERIC_READ;
        allocation_desc.HeapType = D3D12_HEAP_TYPE_READBACK;
    } else {
        initial_state = D3D12_RESOURCE_STATE_COMMON;
        allocation_desc.HeapType = D3D12_HEAP_TYPE_DEFAULT;
    }

    THROW_IF_FAILED(memory_allocator->CreateResource(
        &allocation_desc,
        &resource_desc,
        initial_state,
        nullptr,
        buffer_data.memory_allocation.GetAddressOf(),
        __uuidof(ID3D12Resource), 
        reinterpret_cast<void**>(buffer_data.resource.GetAddressOf()))
    );

    if(flags & BufferFlags::ConstantBuffer) {

        auto constant_buffer_view_desc = D3D12_CONSTANT_BUFFER_VIEW_DESC {};
        constant_buffer_view_desc.BufferLocation = buffer_data.resource->GetGPUVirtualAddress();
        constant_buffer_view_desc.SizeInBytes = static_cast<uint32_t>(size);

        THROW_IF_FAILED(cbv_srv_uav_pool->allocate(buffer_data.descriptor_allocation.GetAddressOf()));

        device->CreateConstantBufferView(&constant_buffer_view_desc, buffer_data.descriptor_allocation->cpu_handle());
    }
    if(flags & BufferFlags::UnorderedAccess) {

        auto unordered_access_view_desc = D3D12_UNORDERED_ACCESS_VIEW_DESC {};
        unordered_access_view_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        unordered_access_view_desc.Format = resource_desc.Format;
        unordered_access_view_desc.Buffer.FirstElement = 0;
        unordered_access_view_desc.Buffer.NumElements = static_cast<uint32_t>(size) / element_stride;
        unordered_access_view_desc.Buffer.StructureByteStride = element_stride;

        THROW_IF_FAILED(cbv_srv_uav_pool->allocate(buffer_data.descriptor_allocation.GetAddressOf()));

        device->CreateUnorderedAccessView(buffer_data.resource.Get(), nullptr, &unordered_access_view_desc, buffer_data.descriptor_allocation->cpu_handle());
    }
    if(flags & BufferFlags::ShaderResource) {

        auto shader_resource_view_desc = D3D12_SHADER_RESOURCE_VIEW_DESC {};
        shader_resource_view_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        shader_resource_view_desc.Format = resource_desc.Format;
        shader_resource_view_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        shader_resource_view_desc.Buffer.FirstElement = 0;
        shader_resource_view_desc.Buffer.NumElements = static_cast<uint32_t>(size) / element_stride;
        shader_resource_view_desc.Buffer.StructureByteStride = element_stride;

        THROW_IF_FAILED(cbv_srv_uav_pool->allocate(buffer_data.descriptor_allocation.GetAddressOf()));

        device->CreateShaderResourceView(buffer_data.resource.Get(), &shader_resource_view_desc, buffer_data.descriptor_allocation->cpu_handle());
    }

    return buffers.push(std::move(buffer_data));
}

void Device::Impl::delete_buffer(Handle<Buffer> const& buffer) {

    buffers.erase(buffer);
}

Handle<RenderPass> Device::Impl::create_render_pass(
    std::span<Handle<Texture> const> const& colors, 
    std::span<RenderPassColorDesc const> const& color_descs, 
    Handle<Texture> const& depth_stencil,
    std::optional<RenderPassDepthStencilDesc> const depth_stencil_desc
) {

    auto get_render_pass_begin_type = [&](RenderPassLoadOp const load_op) {
        switch (load_op) {
    	    case RenderPassLoadOp::Load: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
    	    case RenderPassLoadOp::Clear: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		    case RenderPassLoadOp::DontCare: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
        }
        return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
    };
    
    auto get_render_pass_end_type = [&](RenderPassStoreOp const store_op) {
        switch (store_op) {
    	    case RenderPassStoreOp::Store: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
    	    case RenderPassStoreOp::DontCare: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
        }
        return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
    };

    auto render_pass_data = RenderPass {};
    render_pass_data.render_target_count = static_cast<uint32_t>(colors.size());

    for(uint32_t i = 0; i < render_pass_data.render_target_count; ++i) {

        auto& texture_data = textures[colors[i]];

        auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS {};
        begin.Type = get_render_pass_begin_type(color_descs[i].load_op);
        begin.Clear.ClearValue.Format = texture_data.resource->GetDesc().Format;

        auto end = D3D12_RENDER_PASS_ENDING_ACCESS {};
        end.Type = get_render_pass_end_type(color_descs[i].store_op);

        render_pass_data.render_target_descs[i].BeginningAccess = begin;
        render_pass_data.render_target_descs[i].EndingAccess = end;
        render_pass_data.render_target_descs[i].cpuDescriptor = texture_data.descriptor_allocations[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->cpu_handle();
    }

    if(depth_stencil != InvalidHandle<Texture>()) {

        auto& texture_data = textures[depth_stencil];

        auto depth_begin = D3D12_RENDER_PASS_BEGINNING_ACCESS {};
        depth_begin.Type = get_render_pass_begin_type(depth_stencil_desc.value().depth_load_op);

        // HOTFIX
        // depth_begin.Clear.ClearValue.Format = texture_data.resource->GetDesc().Format;
        depth_begin.Clear.ClearValue.Format = DXGI_FORMAT_D32_FLOAT;

        auto stencil_begin = D3D12_RENDER_PASS_BEGINNING_ACCESS {};
        stencil_begin.Type = get_render_pass_begin_type(depth_stencil_desc.value().stencil_load_op);
        //stencil_begin.Clear.ClearValue.Format = texture_data.resource->GetDesc().Format;
        stencil_begin.Clear.ClearValue.Format = DXGI_FORMAT_D32_FLOAT;

        auto depth_end = D3D12_RENDER_PASS_ENDING_ACCESS {};
        depth_end.Type = get_render_pass_end_type(depth_stencil_desc.value().depth_store_op);

        auto stencil_end = D3D12_RENDER_PASS_ENDING_ACCESS {};
        stencil_end.Type = get_render_pass_end_type(depth_stencil_desc.value().stencil_store_op);

        render_pass_data.depth_stencil_desc.DepthBeginningAccess = depth_begin;
        render_pass_data.depth_stencil_desc.StencilBeginningAccess = stencil_begin;
        render_pass_data.depth_stencil_desc.DepthEndingAccess = depth_end;
        render_pass_data.depth_stencil_desc.StencilEndingAccess = stencil_end;
        render_pass_data.depth_stencil_desc.cpuDescriptor = texture_data.descriptor_allocations[D3D12_DESCRIPTOR_HEAP_TYPE_DSV]->cpu_handle();

        render_pass_data.has_depth_stencil = true;
    }

    return render_passes.push(std::move(render_pass_data));
}

void Device::Impl::delete_render_pass(Handle<RenderPass> const& render_pass) {

    render_passes.erase(render_pass);
}

Handle<Sampler> Device::Impl::create_sampler(
    Filter const filter, 
    AddressMode const address_u, 
    AddressMode const address_v, 
    AddressMode const address_w, 
    uint16_t const anisotropic, 
    CompareOp const compare_op
) {

    auto get_filter = [&](Filter const filter) -> D3D12_FILTER {
        switch(filter) {
            case Filter::Anisotropic: return D3D12_FILTER_ANISOTROPIC;
            case Filter::MinMagMipLinear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
            case Filter::ComparisonMinMagMipLinear: return D3D12_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
        }
        return D3D12_FILTER_ANISOTROPIC;
    };

    auto get_address_mode = [&](AddressMode const address_mode) -> D3D12_TEXTURE_ADDRESS_MODE {
        switch(address_mode) {
            case AddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            case AddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            case AddressMode::Mirror: return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
        }
        return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    };

    auto get_comparison_func = [&](CompareOp const compare_op) {
        switch(compare_op) {
            case CompareOp::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
            case CompareOp::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
            case CompareOp::Greater: return D3D12_COMPARISON_FUNC_GREATER;
            case CompareOp::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
            case CompareOp::Less: return D3D12_COMPARISON_FUNC_LESS;
            case CompareOp::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
            case CompareOp::Never: return D3D12_COMPARISON_FUNC_NEVER;
            case CompareOp::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        }
        return D3D12_COMPARISON_FUNC_ALWAYS;
    };

    auto sampler_data = Sampler {};

    auto sampler_desc = D3D12_SAMPLER_DESC {};
    sampler_desc.Filter = get_filter(filter);
    sampler_desc.AddressU = get_address_mode(address_u);
    sampler_desc.AddressV = get_address_mode(address_v);
    sampler_desc.AddressW = get_address_mode(address_w);
    sampler_desc.ComparisonFunc = get_comparison_func(compare_op);
    sampler_desc.MaxAnisotropy = anisotropic;
    sampler_desc.MaxLOD = D3D12_FLOAT32_MAX;
    
    THROW_IF_FAILED(sampler_pool->allocate(sampler_data.descriptor_allocation.GetAddressOf()));

    device->CreateSampler(&sampler_desc, sampler_data.descriptor_allocation->cpu_handle());

    return samplers.push(std::move(sampler_data));
}

void Device::Impl::delete_sampler(Handle<Sampler> const& sampler) {

    samplers.erase(sampler);
}

Handle<Shader> Device::Impl::create_shader(std::string_view const source, ShaderFlags const flags) {

    auto get_shader_type = [&](ShaderFlags const shader_flags) -> const wchar_t* {
        switch(shader_flags) {
            case ShaderFlags::Vertex: return L"vs_6_0";
            case ShaderFlags::Pixel: return L"ps_6_0";
            case ShaderFlags::Compute: return L"cs_6_0";
            default: return L"vs_6_0";
        }
    };

    ComPtr<IDxcBlobEncoding> source_blob;
    THROW_IF_FAILED(dxc_utils->CreateBlob(source.data(), static_cast<uint32_t>(source.size()), CP_UTF8, source_blob.GetAddressOf()));

    std::vector<const wchar_t*> arguments;
    
    arguments.push_back(L"-E");
    arguments.push_back(L"main");

    arguments.push_back(L"-T");
    arguments.push_back(get_shader_type(flags));

    arguments.push_back(DXC_ARG_WARNINGS_ARE_ERRORS); //-WX
    arguments.push_back(DXC_ARG_DEBUG); //-Zi
    arguments.push_back(DXC_ARG_PACK_MATRIX_COLUMN_MAJOR); //-Zpr

    auto source_buffer = DxcBuffer {};
    source_buffer.Ptr = source_blob->GetBufferPointer();
    source_buffer.Size = source_blob->GetBufferSize();
    source_buffer.Encoding = 0;

    ComPtr<IDxcResult> result;
    THROW_IF_FAILED(dxc_compiler->Compile(
        &source_buffer, 
        arguments.data(), 
        static_cast<uint32_t>(arguments.size()), 
        nullptr, 
        __uuidof(IDxcResult), 
        reinterpret_cast<void**>(result.GetAddressOf()))
    );

    ComPtr<IDxcBlobUtf8> error_blob;
    THROW_IF_FAILED(result->GetOutput(DXC_OUT_ERRORS, __uuidof(IDxcBlobUtf8), reinterpret_cast<void**>(error_blob.GetAddressOf()), nullptr));

    if (error_blob && error_blob->GetStringLength() > 0) {
        std::string_view error_message = std::string_view(error_blob->GetStringPointer(), error_blob->GetStringLength());
        throw lib::Exception(std::format("shader compilation error (\n{})", error_message));
    }

    ComPtr<IDxcBlob> shader_blob;
    THROW_IF_FAILED(result->GetOutput(DXC_OUT_OBJECT, __uuidof(IDxcBlob), reinterpret_cast<void**>(shader_blob.GetAddressOf()), nullptr));

    auto shader_data = Shader {};
    shader_data.flags = flags;

    shader_data.data.resize(shader_blob->GetBufferSize());
    std::memcpy(shader_data.data.data(), shader_blob->GetBufferPointer(), shader_blob->GetBufferSize());
    
    return shaders.push(std::move(shader_data));
}

void Device::Impl::delete_shader(Handle<Shader> const& shader) {

    shaders.erase(shader);
}

Handle<DescriptorLayout> Device::Impl::create_descriptor_layout(std::span<DescriptorLayoutBinding const> const bindings, bool const is_compute) {

    auto get_descriptor_range_type = [&](DescriptorType const descriptor_type) -> D3D12_DESCRIPTOR_RANGE_TYPE {
        switch(descriptor_type) {
            case DescriptorType::ShaderResource: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
            case DescriptorType::ConstantBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
            case DescriptorType::Sampler: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
            case DescriptorType::UnorderedAccess: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
        }
        return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    };

    auto get_shader_visibility = [&](ShaderFlags const shader_flags) -> D3D12_SHADER_VISIBILITY {
        switch(shader_flags) {
            case ShaderFlags::Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
            case ShaderFlags::Pixel: return D3D12_SHADER_VISIBILITY_PIXEL;
            case ShaderFlags::Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
            case ShaderFlags::Domain: return D3D12_SHADER_VISIBILITY_DOMAIN;
            case ShaderFlags::Hull: return D3D12_SHADER_VISIBILITY_HULL;
            case ShaderFlags::All: return D3D12_SHADER_VISIBILITY_ALL;
        }
        return D3D12_SHADER_VISIBILITY_ALL;
    };

    auto descriptor_layout_data = DescriptorLayout {};
    descriptor_layout_data.is_compute = is_compute;

    std::unordered_map<D3D12_DESCRIPTOR_RANGE_TYPE, uint32_t> registers_count;
    registers_count.insert({ D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 0 });
    registers_count.insert({ D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 0 });
    registers_count.insert({ D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 0 });
    registers_count.insert({ D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 0 });

    std::map<std::pair<D3D12_DESCRIPTOR_RANGE_TYPE, D3D12_SHADER_VISIBILITY>, uint32_t> ranges_index;

    std::vector<D3D12_ROOT_PARAMETER> parameters;

    for(auto& binding : bindings) {

        auto it = ranges_index.find({ get_descriptor_range_type(binding.type), get_shader_visibility(binding.flags) });

        if(it != ranges_index.end()) {

            descriptor_layout_data.bindings.insert({ binding.index, { it->second, descriptor_layout_data.ranges.at(it->second).NumDescriptors } });
            ++descriptor_layout_data.ranges.at(it->second).NumDescriptors;

        } else {

            auto range = D3D12_DESCRIPTOR_RANGE {};
            range.RangeType = get_descriptor_range_type(binding.type);
            range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

            descriptor_layout_data.bindings.insert({ binding.index, { static_cast<uint32_t>(descriptor_layout_data.ranges.size()), 0 } });
            ++range.NumDescriptors;

            ranges_index.insert({ { range.RangeType, get_shader_visibility(binding.flags) }, static_cast<uint32_t>(descriptor_layout_data.ranges.size()) });
            descriptor_layout_data.ranges.emplace_back(std::move(range));

            auto parameter = D3D12_ROOT_PARAMETER {};
            parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
            parameter.ShaderVisibility = get_shader_visibility(binding.flags);

            parameters.emplace_back(std::move(parameter));
        }
    }

    for(auto& range : descriptor_layout_data.ranges) {
        auto& register_count = registers_count.at(range.RangeType);
        range.BaseShaderRegister = register_count;
        register_count += range.NumDescriptors;
    }

    for(size_t i = 0; i < parameters.size(); ++i) {
        parameters.at(i).DescriptorTable.pDescriptorRanges = &descriptor_layout_data.ranges.at(i);
        parameters.at(i).DescriptorTable.NumDescriptorRanges = 1;
    }

    auto root_signature_desc = D3D12_ROOT_SIGNATURE_DESC {};
    root_signature_desc.pParameters = parameters.data();
    root_signature_desc.NumParameters = static_cast<uint32_t>(parameters.size());
    root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    ComPtr<ID3DBlob> serialized_blob;
    THROW_IF_FAILED(D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1_0, serialized_blob.GetAddressOf(), nullptr));

    THROW_IF_FAILED(device->CreateRootSignature(
        0,
        serialized_blob->GetBufferPointer(), 
        serialized_blob->GetBufferSize(), 
        __uuidof(ID3D12RootSignature), 
        reinterpret_cast<void**>(descriptor_layout_data.root_signature.GetAddressOf()))
    );

    return descriptor_layouts.push(std::move(descriptor_layout_data));
}

void Device::Impl::delete_descriptor_layout(Handle<DescriptorLayout> const& descriptor_layout) {

    descriptor_layouts.erase(descriptor_layout);
}

Handle<Pipeline> Device::Impl::create_pipeline(
    Handle<DescriptorLayout> const& descriptor_layout,
    std::span<VertexInputDesc const> const vertex_descs,
    std::span<Handle<Shader> const> const shaders,
    RasterizerDesc const& rasterizer_desc,
    DepthStencilDesc const& depth_stencil_desc,
    BlendDesc const& blend_desc,
    Handle<RenderPass> const& render_pass,
    Handle<CachePipeline> const& cache_pipeline
) {

    auto get_fill_mode = [&](FillMode const fill_mode) {
        switch(fill_mode) {
            case FillMode::Solid: return D3D12_FILL_MODE_SOLID;
            case FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
        }
        return D3D12_FILL_MODE_SOLID;
    };

    auto get_cull_mode = [&](CullMode const cull_mode) {
        switch(cull_mode) {
            case CullMode::Front: return D3D12_CULL_MODE_FRONT;
            case CullMode::Back: return D3D12_CULL_MODE_BACK;
            case CullMode::None: return D3D12_CULL_MODE_NONE;
        }
        return D3D12_CULL_MODE_NONE;
    };

    auto get_comparison_func = [&](CompareOp const compare_op) {
        switch(compare_op) {
            case CompareOp::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
            case CompareOp::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
            case CompareOp::Greater: return D3D12_COMPARISON_FUNC_GREATER;
            case CompareOp::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
            case CompareOp::Less: return D3D12_COMPARISON_FUNC_LESS;
            case CompareOp::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
            case CompareOp::Never: return D3D12_COMPARISON_FUNC_NEVER;
            case CompareOp::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
        }
        return D3D12_COMPARISON_FUNC_ALWAYS;
    };

    auto get_blend_func = [&](BlendOp const blend_op) {
        switch(blend_op) {
            case BlendOp::Add: return D3D12_BLEND_OP_ADD;
            case BlendOp::Max: return D3D12_BLEND_OP_MAX;
            case BlendOp::Min: return D3D12_BLEND_OP_MIN;
            case BlendOp::RevSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
            case BlendOp::Subtract: return D3D12_BLEND_OP_SUBTRACT;
        }
        return D3D12_BLEND_OP_ADD;
    };

    auto get_blend = [&](Blend const blend) {
        switch(blend) {
            case Blend::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
            case Blend::One: return D3D12_BLEND_ONE;
            case Blend::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
            case Blend::Zero: return D3D12_BLEND_ZERO;
            case Blend::BlendFactor: return D3D12_BLEND_BLEND_FACTOR;
        }
        return D3D12_BLEND_ZERO;
    };

    auto get_dxgi_format = [&](Format const format) {
        switch(format) {
            case Format::RGB32: return DXGI_FORMAT_R32G32B32_FLOAT;
            case Format::RGBA32: return DXGI_FORMAT_R32G32B32A32_FLOAT;
            case Format::RG32: return DXGI_FORMAT_R32G32_FLOAT;
        }
        return DXGI_FORMAT_R32G32B32_FLOAT;
    };

    auto get_format_size = [&](Format const format) -> uint32_t {
        switch(format) {
            case Format::RGB32: return sizeof(float) * 3;
            case Format::RGBA32: return sizeof(float) * 4;
            case Format::RG32: return sizeof(float) * 2;
        }
        return sizeof(float) * 3;
    };

    auto pipeline_data = Pipeline {};

    auto& descriptor_layout_data = descriptor_layouts[descriptor_layout];
    pipeline_data.root_signature = descriptor_layout_data.root_signature;

    auto pipeline_desc = D3D12_GRAPHICS_PIPELINE_STATE_DESC {};
    pipeline_desc.pRootSignature = pipeline_data.root_signature.Get();

    std::vector<D3D12_INPUT_ELEMENT_DESC> input_element_descs;
    input_element_descs.reserve(vertex_descs.size());

    pipeline_data.vertex_strides.fill(0);

    for(auto const& input : vertex_descs) {

        auto input_element_desc = D3D12_INPUT_ELEMENT_DESC {};
        input_element_desc.SemanticName = input.semantic.c_str();
        input_element_desc.InputSlot = input.slot;
        input_element_desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        input_element_desc.Format = get_dxgi_format(input.format);
        input_element_desc.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
        input_element_descs.emplace_back(input_element_desc);

        pipeline_data.vertex_strides[input.slot] += get_format_size(input.format);
    }

    pipeline_desc.InputLayout.pInputElementDescs = input_element_descs.data();
    pipeline_desc.InputLayout.NumElements = static_cast<uint32_t>(input_element_descs.size());

    auto _rasterizer_desc = D3D12_RASTERIZER_DESC {};
    _rasterizer_desc.FillMode = get_fill_mode(rasterizer_desc.fill_mode);
    _rasterizer_desc.CullMode = get_cull_mode(rasterizer_desc.cull_mode);
    _rasterizer_desc.FrontCounterClockwise = true;
    _rasterizer_desc.DepthBias = 0;
    _rasterizer_desc.DepthBiasClamp = 0.0f;
    _rasterizer_desc.SlopeScaledDepthBias = 0.0f;
    _rasterizer_desc.DepthClipEnable = true;
    _rasterizer_desc.MultisampleEnable = false;
    _rasterizer_desc.AntialiasedLineEnable = false;
    _rasterizer_desc.ForcedSampleCount = 0;
    _rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

    pipeline_desc.RasterizerState = _rasterizer_desc;

    for(auto& shader : shaders) {

        auto& shader_data = this->shaders[shader];
        
        auto shader_bytecode = D3D12_SHADER_BYTECODE {};
        shader_bytecode.pShaderBytecode = shader_data.data.data();
        shader_bytecode.BytecodeLength = shader_data.data.size();

        switch(shader_data.flags) {
            case ShaderFlags::Vertex: pipeline_desc.VS = shader_bytecode; break;
            case ShaderFlags::Pixel: pipeline_desc.PS = shader_bytecode; break;
            case ShaderFlags::Geometry: pipeline_desc.GS = shader_bytecode; break;
            case ShaderFlags::Domain: pipeline_desc.DS = shader_bytecode; break;
            case ShaderFlags::Hull: pipeline_desc.HS = shader_bytecode; break;
        }
    }

    auto _depth_stencil_desc = D3D12_DEPTH_STENCIL_DESC {};
    _depth_stencil_desc.DepthFunc = get_comparison_func(depth_stencil_desc.depth_func);
    _depth_stencil_desc.DepthEnable = depth_stencil_desc.write_enable;
    _depth_stencil_desc.StencilEnable = depth_stencil_desc.write_enable;
    _depth_stencil_desc.DepthWriteMask = depth_stencil_desc.write_enable ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    _depth_stencil_desc.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    _depth_stencil_desc.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;

    auto depth_stencil_face = D3D12_DEPTH_STENCILOP_DESC {};
    depth_stencil_face.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    depth_stencil_face.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    depth_stencil_face.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    depth_stencil_face.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    _depth_stencil_desc.FrontFace = depth_stencil_face;
    _depth_stencil_desc.BackFace = depth_stencil_face;

    pipeline_desc.DepthStencilState = _depth_stencil_desc;

    auto _blend_desc = D3D12_BLEND_DESC {};
    auto render_target_blend_desc = D3D12_RENDER_TARGET_BLEND_DESC {};
    render_target_blend_desc.BlendEnable = blend_desc.blend_enable;
    render_target_blend_desc.SrcBlend = get_blend(blend_desc.blend_src);
    render_target_blend_desc.DestBlend = get_blend(blend_desc.blend_dst);
    render_target_blend_desc.BlendOp = get_blend_func(blend_desc.blend_op);
    render_target_blend_desc.SrcBlendAlpha = get_blend(blend_desc.blend_src_alpha);
    render_target_blend_desc.DestBlendAlpha = get_blend(blend_desc.blend_dst_alpha);
    render_target_blend_desc.BlendOpAlpha = get_blend_func(blend_desc.blend_op_alpha);
    render_target_blend_desc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    auto render_pass_data = render_passes[render_pass];

    for(uint32_t i = 0; i < render_pass_data.render_target_count; ++i) {
        _blend_desc.RenderTarget[i] = render_target_blend_desc;
        pipeline_desc.RTVFormats[i] = render_pass_data.render_target_descs[i].BeginningAccess.Clear.ClearValue.Format;
    }

    // HOTFIX
    pipeline_desc.DSVFormat = render_pass_data.has_depth_stencil ? render_pass_data.depth_stencil_desc.DepthBeginningAccess.Clear.ClearValue.Format : DXGI_FORMAT_UNKNOWN;
    
    pipeline_desc.NumRenderTargets = render_pass_data.render_target_count;
    pipeline_desc.BlendState = _blend_desc;
    pipeline_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    pipeline_desc.SampleMask = std::numeric_limits<uint32_t>::max();

    // TODO!
    pipeline_desc.SampleDesc.Count = 1;

    /*bool is_cached = false;

    if(cache_name.has_value()) {
        
        auto it = shader_cache_data.find(cache_name.value());
        if(it != shader_cache_data.end()) {

            auto cached_pso_state = D3D12_CACHED_PIPELINE_STATE {};
            cached_pso_state.pCachedBlob = it->second->GetBufferPointer();
            cached_pso_state.CachedBlobSizeInBytes = it->second->GetBufferSize();

            pipeline_desc.CachedPSO = cached_pso_state;
            is_cached = true;
        }
    }*/

    THROW_IF_FAILED(device->CreateGraphicsPipelineState(
        &pipeline_desc,
        __uuidof(ID3D12PipelineState), 
        reinterpret_cast<void**>(pipeline_data.pipeline_state.GetAddressOf()))
    );

    /*if(!is_cached && cache_name.has_value()) {

        ComPtr<ID3DBlob> buffer;
        pipeline_data.pipeline_state->GetCachedBlob(buffer.GetAddressOf());
        
        shader_cache_data[cache_name.value()] = buffer;
        is_cached = true;
    }*/

    return pipelines.push(std::move(pipeline_data));
}

Handle<Pipeline> Device::Impl::create_pipeline(
    Handle<DescriptorLayout> const& descriptor_layout,
    Handle<Shader> const& shader,
    Handle<CachePipeline> const& cache_pipeline
) {
    auto pipeline_data = Pipeline {};
    pipeline_data.is_compute = true;

    auto& descriptor_layout_data = descriptor_layouts[descriptor_layout];
    pipeline_data.root_signature = descriptor_layout_data.root_signature;

    auto pipeline_desc = D3D12_COMPUTE_PIPELINE_STATE_DESC {};
    pipeline_desc.pRootSignature = pipeline_data.root_signature.Get();

    auto& shader_data = this->shaders[shader];
        
    auto shader_bytecode = D3D12_SHADER_BYTECODE {};
    shader_bytecode.pShaderBytecode = shader_data.data.data();
    shader_bytecode.BytecodeLength = shader_data.data.size();

    pipeline_desc.CS = shader_bytecode;

    /*bool is_cached = false;

    if(cache_name.has_value()) {
        
        auto it = shader_cache_data.find(cache_name.value());
        if(it != shader_cache_data.end()) {

            auto cached_pso_state = D3D12_CACHED_PIPELINE_STATE {};
            cached_pso_state.pCachedBlob = it->second->GetBufferPointer();
            cached_pso_state.CachedBlobSizeInBytes = it->second->GetBufferSize();

            pipeline_desc.CachedPSO = cached_pso_state;
            is_cached = true;
        }
    }*/

    THROW_IF_FAILED(device->CreateComputePipelineState(
        &pipeline_desc,
        __uuidof(ID3D12PipelineState), 
        reinterpret_cast<void**>(pipeline_data.pipeline_state.GetAddressOf()))
    );

    /*if(!is_cached && cache_name.has_value()) {

        ComPtr<ID3DBlob> buffer;
        pipeline_data.pipeline_state->GetCachedBlob(buffer.GetAddressOf());
        
        shader_cache_data[cache_name.value()] = buffer;
        is_cached = true;
    }*/

    return pipelines.push(std::move(pipeline_data));
}

void Device::Impl::delete_pipeline(Handle<Pipeline> const& pipeline) {

    pipelines.erase(pipeline);
}

Handle<CommandList> Device::Impl::create_command_list(QueueFlags const flags, bool const bundle) {

    auto get_command_list_type = [&](QueueFlags const queue_flags) -> D3D12_COMMAND_LIST_TYPE {
        switch(queue_flags) {
            case QueueFlags::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
            case QueueFlags::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
            case QueueFlags::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
        }
        return D3D12_COMMAND_LIST_TYPE_DIRECT;
    };

    auto command_list_data = CommandList {};

    if(bundle) {

        THROW_IF_FAILED(device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_BUNDLE, 
            __uuidof(ID3D12CommandAllocator), 
            reinterpret_cast<void**>(command_list_data.command_allocator.GetAddressOf()))
        );
            
        THROW_IF_FAILED(device->CreateCommandList1(
            0, 
            D3D12_COMMAND_LIST_TYPE_BUNDLE, 
            D3D12_COMMAND_LIST_FLAG_NONE,
            __uuidof(ID3D12GraphicsCommandList4),
            reinterpret_cast<void**>(command_list_data.command_list.GetAddressOf()))
        );

    } else {

        THROW_IF_FAILED(device->CreateCommandAllocator(
            get_command_list_type(flags), 
            __uuidof(ID3D12CommandAllocator), 
            reinterpret_cast<void**>(command_list_data.command_allocator.GetAddressOf()))
        );
            
        THROW_IF_FAILED(device->CreateCommandList1(
            0, 
            get_command_list_type(flags), 
            D3D12_COMMAND_LIST_FLAG_NONE,
            __uuidof(ID3D12GraphicsCommandList4),
            reinterpret_cast<void**>(command_list_data.command_list.GetAddressOf()))
        );
    }

    return command_lists.push(std::move(command_list_data));
}

void Device::Impl::delete_command_list(Handle<CommandList> const& command_list) {

    command_lists.erase(command_list);
}

uint8_t* Device::Impl::map_buffer_data(Handle<Buffer> const& buffer, uint64_t const offset) {

    auto& buffer_data = buffers[buffer];

    auto properties = D3D12_HEAP_PROPERTIES {};
    buffer_data.memory_allocation->GetResource()->GetHeapProperties(&properties, nullptr);

    uint8_t* bytes = nullptr;

    if(properties.Type == D3D12_HEAP_TYPE_UPLOAD || properties.Type == D3D12_HEAP_TYPE_READBACK) {
        auto range = D3D12_RANGE {};
        THROW_IF_FAILED(buffer_data.resource->Map(0, &range, reinterpret_cast<void**>(&bytes)));
        bytes += offset;
    } else {
        assert(false && "invalid buffer heap type");
    }
    return bytes;
}

void Device::Impl::unmap_buffer_data(Handle<Buffer> const& buffer) {
    auto& buffer_data = buffers[buffer];
    auto range = D3D12_RANGE {};
    buffer_data.resource->Unmap(0, &range);
}

size_t Device::Impl::get_memory_required_size(ResourceHandle const& resource) {

    size_t total_bytes = 0;

    auto resource_visitor = make_visitor(
        [&](Handle<Texture> const& texture) {
            auto& texture_data = textures[texture];
            auto resource_desc = texture_data.resource->GetDesc();

            device->GetCopyableFootprints(
                &resource_desc,
                0,
                resource_desc.MipLevels,
                0,
                nullptr,
                nullptr,
                nullptr,
                &total_bytes
            );
        },
        [&](Handle<Buffer> const& buffer) {
            auto& texture_data = buffers[buffer];
            auto resource_desc = texture_data.resource->GetDesc();

            device->GetCopyableFootprints(
                &resource_desc,
                0,
                resource_desc.MipLevels,
                0,
                nullptr,
                nullptr,
                nullptr,
                &total_bytes
            );
        },
        [&](Handle<Sampler> const& sampler) { }
    );

    std::visit(resource_visitor, resource);

    return total_bytes;
}

uint32_t Device::Impl::acquire_next_swapchain_texture() {
    swapchain_index = swapchain->GetCurrentBackBufferIndex();
    cbv_srv_uav_ranges[swapchain_index]->reset();
    sampler_ranges[swapchain_index]->reset();
    return swapchain_index;
}

Handle<Texture> Device::Impl::swapchain_texture(uint32_t const index) {
    return Handle<Texture>(swapchain_textures[index]);
}

void Device::Impl::present() {
    swapchain->Present(0, 0);
}

void Device::Impl::recreate_swapchain(uint32_t const width, uint32_t const height, std::optional<SwapchainDesc> swapchain_desc) {

    for(auto const& texture : swapchain_textures) {
        delete_texture(texture);
    }
    swapchain_textures.clear();

    auto _swapchain_desc = DXGI_SWAP_CHAIN_DESC1 {};
    swapchain->GetDesc1(&_swapchain_desc);

    THROW_IF_FAILED(swapchain->ResizeBuffers(_swapchain_desc.BufferCount, width, height, _swapchain_desc.Format, 0));
    swapchain_textures.resize(_swapchain_desc.BufferCount);

    // Get Swapchain resources and create RTV's
    {
        for(uint32_t i = 0; i < _swapchain_desc.BufferCount; ++i) {

            auto texture = Texture {};

            THROW_IF_FAILED(swapchain->GetBuffer(i, __uuidof(ID3D12Resource), reinterpret_cast<void**>(texture.resource.GetAddressOf())));

            auto render_target_view_desc = D3D12_RENDER_TARGET_VIEW_DESC {};
            render_target_view_desc.Format = _swapchain_desc.Format;
            render_target_view_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

            auto& cur_allocation = texture.descriptor_allocations[D3D12_DESCRIPTOR_HEAP_TYPE_RTV];
            THROW_IF_FAILED(rtv_pool->allocate(cur_allocation.GetAddressOf()));

            device->CreateRenderTargetView(texture.resource.Get(), &render_target_view_desc, cur_allocation->cpu_handle());

            swapchain_textures[i] = textures.push(std::move(texture));
        }
    }

    swapchain_index = 0;
}

uint64_t Device::Impl::submit(std::span<Handle<CommandList> const> const command_lists, QueueFlags const flags) {

    auto get_queue_index = [&](QueueFlags const queue_flags) -> uint32_t {
        switch(queue_flags) {
            case QueueFlags::Graphics: return 0;
            case QueueFlags::Copy: return 1;
            case QueueFlags::Compute: return 2;
        }
        return 0;
    };

    batches.clear();

    for(auto& command_list : command_lists) {
        auto& command_list_data = this->command_lists[command_list];
        command_list_data.command_list->Close();
        command_list_data.is_reset = false;
        batches.push_back(reinterpret_cast<ID3D12CommandList* const>(command_list_data.command_list.Get()));
    }

    queues[get_queue_index(flags)]->ExecuteCommandLists(static_cast<uint32_t>(batches.size()), batches.data());

    uint64_t fence_value_result = fence_values[get_queue_index(flags)].load();
    ++fence_values[get_queue_index(flags)];

    THROW_IF_FAILED(queues[get_queue_index(flags)]->Signal(fences[get_queue_index(flags)].Get(), fence_value_result));

    return fence_value_result;
}

uint64_t Device::Impl::submit_after(std::span<Handle<CommandList> const> const command_lists, uint64_t fence_value, QueueFlags const flags) {
    
    auto get_queue_index = [&](QueueFlags const queue_flags) -> uint32_t {
        switch(queue_flags) {
            case QueueFlags::Graphics: return 0;
            case QueueFlags::Copy: return 1;
            case QueueFlags::Compute: return 2;
        }
        return 0;
    };

    batches.clear();

    for(auto& command_list : command_lists) {
        auto& command_list_data = this->command_lists[command_list];
        command_list_data.command_list->Close();
        command_list_data.is_reset = false;
        batches.push_back(reinterpret_cast<ID3D12CommandList* const>(command_list_data.command_list.GetAddressOf()));
    }

    THROW_IF_FAILED(queues[get_queue_index(flags)]->Wait(fences[get_queue_index(flags)].Get(), fence_value));
    queues[get_queue_index(flags)]->ExecuteCommandLists(static_cast<uint32_t>(batches.size()), batches.data());

    uint64_t fence_value_result = fence_values[get_queue_index(flags)].load();
    ++fence_values[get_queue_index(flags)];

    THROW_IF_FAILED(queues[get_queue_index(flags)]->Signal(fences[get_queue_index(flags)].Get(), fence_value_result));

    return fence_value_result;
}

void Device::Impl::wait(uint64_t const fence_value, QueueFlags const flags) {
    
    auto get_queue_index = [&](QueueFlags const queue_flags) -> uint32_t {
        switch(queue_flags) {
            case QueueFlags::Graphics: return 0;
            case QueueFlags::Copy: return 1;
            case QueueFlags::Compute: return 2;
        }
        return 0;
    };

    if(fences[get_queue_index(flags)]->GetCompletedValue() < fence_value) {
        THROW_IF_FAILED(fences[get_queue_index(flags)]->SetEventOnCompletion(fence_value, fence_event));
        WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
    }
}

bool Device::Impl::is_completed(uint64_t const fence_value, QueueFlags const flags) const {

    auto get_queue_index = [&](QueueFlags const queue_flags) -> uint32_t {
        switch(queue_flags) {
            case QueueFlags::Graphics: return 0;
            case QueueFlags::Copy: return 1;
            case QueueFlags::Compute: return 2;
        }
        return 0;
    };
    
    return fences[get_queue_index(flags)]->GetCompletedValue() >= fence_value;
}

void Device::Impl::wait_for_idle(QueueFlags const flags) {

    if(flags & QueueFlags::Graphics) {

        uint64_t fence_value = fence_values[0].load();
        ++fence_values[0];

        THROW_IF_FAILED(queues[0]->Signal(fences[0].Get(), fence_value));
        THROW_IF_FAILED(fences[0]->SetEventOnCompletion(fence_value, fence_event));

        WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
    }

    if(flags & QueueFlags::Copy) {

        uint64_t fence_value = fence_values[1].load();
        ++fence_values[1];

        THROW_IF_FAILED(queues[1]->Signal(fences[1].Get(), fence_value));
        THROW_IF_FAILED(fences[1]->SetEventOnCompletion(fence_value, fence_event));

        WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
    }

    if(flags & QueueFlags::Compute) {

        uint64_t fence_value = fence_values[2].load();
        ++fence_values[2];

        THROW_IF_FAILED(queues[2]->Signal(fences[2].Get(), fence_value));
        THROW_IF_FAILED(fences[2]->SetEventOnCompletion(fence_value, fence_event));

        WaitForSingleObjectEx(fence_event, INFINITE, FALSE);
    }
}

void Device::Impl::command_list_reset(CommandList& command_list) {

    THROW_IF_FAILED(command_list.command_allocator->Reset());
    THROW_IF_FAILED(command_list.command_list->Reset(command_list.command_allocator.Get(), nullptr));

    switch(command_list.command_list->GetType()) {
        case D3D12_COMMAND_LIST_TYPE_DIRECT:
        case D3D12_COMMAND_LIST_TYPE_BUNDLE:
        case D3D12_COMMAND_LIST_TYPE_COMPUTE: {
            std::array<ID3D12DescriptorHeap*, 2> descriptor_heaps;
            descriptor_heaps[0] = cbv_srv_uav_ranges[swapchain_index]->heap();
            descriptor_heaps[1] = sampler_ranges[swapchain_index]->heap();

            command_list.command_list->SetDescriptorHeaps(2, descriptor_heaps.data());
        } break;
    }

    command_list.is_reset = true;
}

void Device::Impl::set_viewport(Handle<CommandList> const& command_list, int32_t const x, int32_t const y, uint32_t const width, uint32_t const height) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    auto viewport = D3D12_VIEWPORT {};
    viewport.TopLeftX = static_cast<float>(x);
    viewport.TopLeftY = static_cast<float>(y);
    viewport.Width = static_cast<float>(width);
    viewport.Height = static_cast<float>(height);
    viewport.MinDepth = D3D12_MIN_DEPTH;
    viewport.MaxDepth = D3D12_MAX_DEPTH;

    command_list_data.command_list->RSSetViewports(1, &viewport);
}

void Device::Impl::set_scissor(Handle<CommandList> const& command_list, int32_t const left, int32_t const top, int32_t const right, int32_t const bottom) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    auto rect = D3D12_RECT {};
    rect.top = static_cast<LONG>(top);
    rect.bottom = static_cast<LONG>(bottom);
    rect.left = static_cast<LONG>(left);
    rect.right = static_cast<LONG>(right);

    command_list_data.command_list->RSSetScissorRects(1, &rect);
}

void Device::Impl::barrier(Handle<CommandList> const& command_list, std::span<MemoryBarrierDesc const> const barriers) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    auto get_memory_state = [&](MemoryState const memory_state) -> D3D12_RESOURCE_STATES {
        switch(memory_state) {
		    case MemoryState::Common: return D3D12_RESOURCE_STATE_COMMON;
		    case MemoryState::RenderTarget: return D3D12_RESOURCE_STATE_RENDER_TARGET;
		    case MemoryState::Present: return D3D12_RESOURCE_STATE_PRESENT;
		    case MemoryState::CopySource: return D3D12_RESOURCE_STATE_COPY_SOURCE;
		    case MemoryState::CopyDest: return D3D12_RESOURCE_STATE_COPY_DEST;
            case MemoryState::ShaderRead: return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
            case MemoryState::VertexConstantBufferRead: return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
            case MemoryState::IndexBufferRead: return D3D12_RESOURCE_STATE_INDEX_BUFFER;
            case MemoryState::DepthWrite: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
            case MemoryState::DepthRead: return D3D12_RESOURCE_STATE_DEPTH_READ;
            case MemoryState::GenericRead: return D3D12_RESOURCE_STATE_GENERIC_READ;
            case MemoryState::NonPixelShaderRead: return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            case MemoryState::UnorderedAccess: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
        }
        return D3D12_RESOURCE_STATE_COMMON;
    };

    std::vector<D3D12_RESOURCE_BARRIER> resource_barriers;
    for(auto const& barrier : barriers) {

        auto resource_barrier = D3D12_RESOURCE_BARRIER {};
        resource_barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        resource_barrier.Transition.StateBefore = get_memory_state(barrier.before);
        resource_barrier.Transition.StateAfter = get_memory_state(barrier.after);

        auto barrier_visitor = make_visitor(
            [&](Handle<Texture> const& target) {
                resource_barrier.Transition.pResource = textures[target].resource.Get();
            },
            [&](Handle<Buffer> const& target) {
                resource_barrier.Transition.pResource = buffers[target].resource.Get();
            },
            [&](Handle<Sampler> const& target) { }
        );

        std::visit(barrier_visitor, barrier.target);

        resource_barriers.emplace_back(std::move(resource_barrier));
    }

    command_list_data.command_list->ResourceBarrier(static_cast<uint32_t>(resource_barriers.size()), resource_barriers.data());
}

void Device::Impl::begin_render_pass(
    Handle<CommandList> const& command_list,
    Handle<RenderPass> const& render_pass, 
    std::span<lib::math::Color const> const clear_colors, 
    float const clear_depth, 
    uint8_t const clear_stencil
) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    auto& render_pass_data = render_passes[render_pass];

    for(uint32_t i = 0; i < render_pass_data.render_target_count; ++i) {
        std::memcpy(render_pass_data.render_target_descs[i].BeginningAccess.Clear.ClearValue.Color, &clear_colors[i], sizeof(lib::math::Color));
    }

    if(render_pass_data.has_depth_stencil) {

        render_pass_data.depth_stencil_desc.DepthBeginningAccess.Clear.ClearValue.DepthStencil.Depth = clear_depth;
        render_pass_data.depth_stencil_desc.StencilBeginningAccess.Clear.ClearValue.DepthStencil.Stencil = clear_stencil;

        command_list_data.command_list->BeginRenderPass(
            render_pass_data.render_target_count, 
            render_pass_data.render_target_descs.data(), 
            &render_pass_data.depth_stencil_desc, 
            D3D12_RENDER_PASS_FLAG_NONE
        );
    } else {
        command_list_data.command_list->BeginRenderPass(
            render_pass_data.render_target_count, 
            render_pass_data.render_target_descs.data(), 
            nullptr, 
            D3D12_RENDER_PASS_FLAG_NONE
        );
    }
}

void Device::Impl::end_render_pass(Handle<CommandList> const& command_list) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    command_list_data.command_list->EndRenderPass();
}

void Device::Impl::begin_command_list(Handle<CommandList> const& command_list) {

}

void Device::Impl::close_command_list(Handle<CommandList> const& command_list) {
    auto& command_list_data = command_lists[command_list];
    THROW_IF_FAILED(command_list_data.command_list->Close());
    command_list_data.is_reset = false;
    command_list_data.binded_pipeline = nullptr;
}

void Device::Impl::execute_bundle(Handle<CommandList> const& command_list, Handle<CommandList> const& bundle_command_list) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    auto& bundle_command_list_data = command_lists[bundle_command_list];

    command_list_data.command_list->ExecuteBundle(bundle_command_list_data.command_list.Get());
}

void Device::Impl::dispatch(Handle<CommandList> const& command_list, uint32_t const thread_group_x, uint32_t const thread_group_y, uint32_t const thread_group_z) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    command_list_data.command_list->Dispatch(thread_group_x, thread_group_y, thread_group_z);
}

void Device::Impl::bind_pipeline(Handle<CommandList> const& command_list, Handle<Pipeline> const& pipeline) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    auto& pipeline_data = pipelines[pipeline];

    command_list_data.binded_pipeline = &pipeline_data;

    command_list_data.command_list->SetPipelineState(pipeline_data.pipeline_state.Get());

    if(!pipeline_data.is_compute) {
        command_list_data.command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        command_list_data.command_list->SetGraphicsRootSignature(pipeline_data.root_signature.Get());
    } else {
        command_list_data.command_list->SetComputeRootSignature(pipeline_data.root_signature.Get());
    }
}

void Device::Impl::bind_resources(Handle<CommandList> const& command_list, Handle<DescriptorLayout> const& descriptor_layout, std::span<DescriptorWriteDesc const> const write_descs) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    auto& descriptor_layout_data = descriptor_layouts[descriptor_layout];

    std::vector<ComPtr<d3d12::DescriptorAllocation>> allocations(descriptor_layout_data.ranges.size());

    for(size_t i = 0; i < allocations.size(); ++i) {

        switch(descriptor_layout_data.ranges.at(i).RangeType) {
            case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
            case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
            case D3D12_DESCRIPTOR_RANGE_TYPE_UAV: {
                THROW_IF_FAILED(cbv_srv_uav_ranges[swapchain_index]->allocate(descriptor_layout_data.ranges.at(i), allocations.at(i).GetAddressOf()));
            } break;
            case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER: {
                THROW_IF_FAILED(sampler_ranges[swapchain_index]->allocate(descriptor_layout_data.ranges.at(i), allocations.at(i).GetAddressOf()));
            } break;
        }

        if(!descriptor_layout_data.is_compute) {
            command_list_data.command_list->SetGraphicsRootDescriptorTable(static_cast<uint32_t>(i), allocations.at(i)->gpu_handle());
        } else {
            command_list_data.command_list->SetComputeRootDescriptorTable(static_cast<uint32_t>(i), allocations.at(i)->gpu_handle());   
        }
    }
    
    for(auto& write_desc : write_descs) {

        ComPtr<d3d12::DescriptorAllocation> allocation;
        D3D12_DESCRIPTOR_HEAP_TYPE heap_type;

        auto resource_visitor = make_visitor(
            [&](Handle<Texture> const& resource) {
                auto& texture_data = textures[resource];
                allocation = texture_data.descriptor_allocations[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV];
                heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            },
            [&](Handle<Buffer> const& resource) {
                auto& buffer_data = buffers[resource];
                allocation = buffer_data.descriptor_allocation;
                heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
            },
            [&](Handle<Sampler> const& resource) {
                auto& sampler_data = samplers[resource];
                allocation = sampler_data.descriptor_allocation;
                heap_type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
            }
        );

        std::visit(resource_visitor, write_desc.data);

        auto const& binding_location = descriptor_layout_data.bindings.at(write_desc.index);

        uint32_t descriptor_size = device->GetDescriptorHandleIncrementSize(heap_type);

        auto cpu_handle = allocations[binding_location.range_index]->cpu_handle();
        cpu_handle.ptr += descriptor_size * binding_location.offset;

        device->CopyDescriptorsSimple(
            1,
            cpu_handle,
            allocation->cpu_handle(),
            heap_type
        );
    }
}

void Device::Impl::bind_vertex_buffer(Handle<CommandList> const& command_list, uint32_t const index, Handle<Buffer> const& buffer, uint64_t const offset) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    auto& buffer_data = buffers[buffer];

    auto vertex_view = D3D12_VERTEX_BUFFER_VIEW {};
    vertex_view.BufferLocation = buffer_data.resource->GetGPUVirtualAddress() + offset;
    vertex_view.SizeInBytes = static_cast<uint32_t>(buffer_data.resource->GetDesc().Width);
    vertex_view.StrideInBytes = command_list_data.binded_pipeline->vertex_strides[index];

    command_list_data.command_list->IASetVertexBuffers(index, 1, &vertex_view);
}

void Device::Impl::bind_index_buffer(Handle<CommandList> const& command_list, Handle<Buffer> const& buffer, uint64_t const offset) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    auto& buffer_data = buffers[buffer];

    auto index_view = D3D12_INDEX_BUFFER_VIEW {};
    index_view.BufferLocation = buffer_data.resource->GetGPUVirtualAddress() + offset;
    index_view.SizeInBytes = static_cast<uint32_t>(buffer_data.resource->GetDesc().Width);
    index_view.Format = DXGI_FORMAT_R32_UINT;

    command_list_data.command_list->IASetIndexBuffer(&index_view);
}

void Device::Impl::copy_texture_region(
    Handle<CommandList> const& command_list,
    Handle<Texture> const& dest,
    Handle<Buffer> const& source,
    std::span<TextureCopyRegion const> const regions
) {
    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    auto& texture_data = textures[dest];
    auto& source_buffer_data = buffers[source];

    auto resource_desc = texture_data.resource->GetDesc();

    auto result = std::min_element(regions.begin(), regions.end(), [&](auto const& lhs, auto const& rhs) { return lhs.mip_index < rhs.mip_index; });

    std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> footprints(regions.size());

    device->GetCopyableFootprints(
        &resource_desc,
        result->mip_index,
        static_cast<uint32_t>(regions.size()),
        0,
        footprints.data(),
        nullptr,
        nullptr,
        nullptr
    );

    for(auto const& region : regions) {

        footprints[region.mip_index].Offset = region.offset;
        footprints[region.mip_index].Footprint.RowPitch = region.row_pitch;

        auto source_location = D3D12_TEXTURE_COPY_LOCATION {};
        source_location.pResource = source_buffer_data.resource.Get();
        source_location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        source_location.PlacedFootprint = footprints[region.mip_index];

        auto dest_location = D3D12_TEXTURE_COPY_LOCATION {};
        dest_location.pResource = texture_data.resource.Get();
        dest_location.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        dest_location.SubresourceIndex = region.mip_index;

        command_list_data.command_list->CopyTextureRegion(&dest_location, 0, 0, 0, &source_location, nullptr);
    }
}

void Device::Impl::copy_buffer_region(
    Handle<CommandList> const& command_list,
    Handle<Buffer> const& dest, 
    uint64_t const dest_offset, 
    Handle<Buffer> const& source, 
    uint64_t const source_offset,
    size_t const size
) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    auto& dest_buffer = buffers[dest];
    auto& source_buffer = buffers[source];

    command_list_data.command_list->CopyBufferRegion(
        dest_buffer.resource.Get(), 
        dest_offset,
        source_buffer.resource.Get(),
        source_offset,
        size
    );
}

void Device::Impl::draw(Handle<CommandList> const& command_list, uint32_t const vertex_count, uint32_t const instance_count, uint32_t const vertex_offset) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    command_list_data.command_list->DrawInstanced(vertex_count, instance_count, vertex_offset, 0);
}

void Device::Impl::draw_indexed(Handle<CommandList> const& command_list, uint32_t const index_count, uint32_t const instance_count, uint32_t const instance_offset) {

    auto& command_list_data = command_lists[command_list];

    if(!command_list_data.is_reset) {
        command_list_reset(command_list_data);
    }

    command_list_data.command_list->DrawIndexedInstanced(index_count, instance_count, 0, 0, instance_offset);
}

AdapterDesc Device::Impl::adapter_desc() const {

    auto adapter_desc = DXGI_ADAPTER_DESC1 {};
    adapter->GetDesc1(&adapter_desc);
    
    auto local_budget = D3D12MA::Budget {};
    memory_allocator->GetBudget(&local_budget, nullptr);

    size_t length = wcslen(adapter_desc.Description) + 1;
    size_t result = 0;
	
	std::string out_str(length - 1, 0);

    wcstombs_s(&result, reinterpret_cast<char*>(out_str.data()), length, reinterpret_cast<const wchar_t*>(adapter_desc.Description), length - 1);

    return AdapterDesc {
        .name = out_str,
        .local_memory_size = adapter_desc.DedicatedVideoMemory,
        .local_memory_usage = local_budget.UsageBytes
    };
}

//===========================================================
//
//
//          Backend Pointer to Implementation
//
//
//===========================================================

Device::Device(uint32_t const adapter_index, SwapchainDesc const& swapchain_desc) : 
    _impl(std::unique_ptr<Impl, impl_deleter>(new Impl(
        std::to_underlying(BackendLimits::Textures),
        std::to_underlying(BackendLimits::Buffers),
        std::to_underlying(BackendLimits::Samplers),
        std::to_underlying(BackendLimits::DescriptorLayouts),
        std::to_underlying(BackendLimits::Pipelines),
        std::to_underlying(BackendLimits::Shaders),
        std::to_underlying(BackendLimits::RenderPasses),
        std::to_underlying(BackendLimits::CommandLists)))
    ) {

    _impl->initialize(adapter_index, swapchain_desc);
}

Device::~Device() {

    _impl->deinitialize();
}

Handle<Texture> Device::create_texture(
    Dimension const dimension,
    uint32_t const width,
    uint32_t const height,
    uint16_t const mip_levels,
    uint16_t const array_layers,
    Format const format,
    TextureFlags const flags
) {
    
    return _impl->create_texture(dimension, width, height, mip_levels, array_layers, format, flags);
}

void Device::delete_texture(Handle<Texture> const& texture) {

    _impl->delete_texture(texture);
}

Handle<Buffer> Device::create_buffer(size_t const size, BufferFlags const flags, uint32_t const element_stride) {

    return _impl->create_buffer(size, flags, element_stride); 
}

void Device::delete_buffer(Handle<Buffer> const& buffer) {

    _impl->delete_buffer(buffer);
}

Handle<RenderPass> Device::create_render_pass(
    std::span<Handle<Texture> const> const& colors, 
    std::span<RenderPassColorDesc const> const& color_descs, 
    Handle<Texture> const& depth_stencil,
    std::optional<RenderPassDepthStencilDesc> const depth_stencil_desc
) {
    return _impl->create_render_pass(colors, color_descs, depth_stencil, depth_stencil_desc);
}

void Device::delete_render_pass(Handle<RenderPass> const& render_pass) {

    _impl->delete_render_pass(render_pass);
}

Handle<Sampler> Device::create_sampler(
    Filter const filter, 
    AddressMode const address_u, 
    AddressMode const address_v, 
    AddressMode const address_w, 
    uint16_t const anisotropic, 
    CompareOp const compare_op
) {

    return _impl->create_sampler(filter, address_u, address_v, address_w, anisotropic, compare_op);
}

void Device::delete_sampler(Handle<Sampler> const& sampler) {

    _impl->delete_sampler(sampler);
}

Handle<Shader> Device::create_shader(std::string_view const source, ShaderFlags const flags) {

    return _impl->create_shader(source, flags);
}

void Device::delete_shader(Handle<Shader> const& shader) {

    _impl->delete_shader(shader);
}

Handle<DescriptorLayout> Device::create_descriptor_layout(std::span<DescriptorLayoutBinding const> const bindings, bool const is_compute) {

    return _impl->create_descriptor_layout(bindings, is_compute);
}

void Device::delete_descriptor_layout(Handle<DescriptorLayout> const& descriptor_layout) {

    _impl->delete_descriptor_layout(descriptor_layout);
}

Handle<Pipeline> Device::create_pipeline(
    Handle<DescriptorLayout> const& descriptor_layout,
    std::span<VertexInputDesc const> const vertex_descs,
    std::span<Handle<Shader> const> const shaders,
    RasterizerDesc const& rasterizer_desc,
    DepthStencilDesc const& depth_stencil_desc,
    BlendDesc const& blend_desc,
    Handle<RenderPass> const& render_pass,
    Handle<CachePipeline> const& cache_pipeline
) {
    return _impl->create_pipeline(descriptor_layout, vertex_descs, shaders, rasterizer_desc, depth_stencil_desc, blend_desc, render_pass, cache_pipeline);
}

Handle<Pipeline> Device::create_pipeline(
    Handle<DescriptorLayout> const& descriptor_layout,
    Handle<Shader> const& shader,
    Handle<CachePipeline> const& cache_pipeline
) {
    return _impl->create_pipeline(descriptor_layout, shader, cache_pipeline);
}

void Device::delete_pipeline(Handle<Pipeline> const& pipeline) {

    _impl->delete_pipeline(pipeline);
}

Handle<CommandList> Device::create_command_list(QueueFlags const flags, bool const bundle) {

    return _impl->create_command_list(flags, bundle);
}

void Device::delete_command_list(Handle<CommandList> const& command_list) {

    _impl->delete_command_list(command_list);
}

uint8_t* Device::map_buffer_data(Handle<Buffer> const& buffer, uint64_t const offset) {
    return _impl->map_buffer_data(buffer, offset);
}

void Device::unmap_buffer_data(Handle<Buffer> const& buffer) {
    _impl->unmap_buffer_data(buffer);
}

void Device::present() {
    _impl->present();
}

size_t Device::get_memory_required_size(ResourceHandle const& resource) {
    return _impl->get_memory_required_size(resource);
}

uint32_t Device::acquire_next_swapchain_texture() {
    return _impl->acquire_next_swapchain_texture();
}

Handle<Texture> Device::swapchain_texture(uint32_t const index) {
    return _impl->swapchain_texture(index);
}

void Device::recreate_swapchain(uint32_t const width, uint32_t const height, std::optional<SwapchainDesc> swapchain_desc) {
    
    _impl->recreate_swapchain(width, height, swapchain_desc);
}

uint64_t Device::submit(std::span<Handle<CommandList> const> const command_lists, QueueFlags const flags) {

    return _impl->submit(command_lists, flags);
}

uint64_t Device::submit_after(std::span<Handle<CommandList> const> const command_lists, uint64_t const fence_result, QueueFlags const flags) {
    
    return _impl->submit_after(command_lists, fence_result, flags);
}

void Device::wait(uint64_t const fence_result, QueueFlags const flags) {
    
    return _impl->wait(fence_result, flags);
}

bool Device::is_completed(uint64_t const fence_result, QueueFlags const flags) const {
    
    return _impl->is_completed(fence_result, flags);
}

void Device::wait_for_idle(QueueFlags const flags) {
    return _impl->wait_for_idle(flags);
}

void Device::set_viewport(Handle<CommandList> const& command_list, int32_t const x, int32_t const y, uint32_t const width, uint32_t const height) {
    _impl->set_viewport(command_list, x, y, width, height);
}

void Device::set_scissor(Handle<CommandList> const& command_list, int32_t const left, int32_t const top, int32_t const right, int32_t const bottom) {
    _impl->set_scissor(command_list, left, top, right, bottom);
}

void Device::barrier(Handle<CommandList> const& command_list, std::span<MemoryBarrierDesc const> const barriers) {
    _impl->barrier(command_list, barriers);
}

void Device::begin_render_pass(
    Handle<CommandList> const& command_list, 
    Handle<RenderPass> const& render_pass, 
    std::span<lib::math::Color const> const clear_colors, 
    float const clear_depth, 
    uint8_t const clear_stencil
) {
    _impl->begin_render_pass(command_list, render_pass, clear_colors, clear_depth, clear_stencil);
}

void Device::end_render_pass(Handle<CommandList> const& command_list) {
    _impl->end_render_pass(command_list);
}

void Device::begin_command_list(Handle<CommandList> const& command_list) {
    _impl->begin_command_list(command_list);
}

void Device::close_command_list(Handle<CommandList> const& command_list) {
    _impl->close_command_list(command_list);
}

void Device::execute_bundle(Handle<CommandList> const& command_list, Handle<CommandList> const& bundle_command_list) {
    _impl->execute_bundle(command_list, bundle_command_list);
}

void Device::dispatch(Handle<CommandList> const& command_list, uint32_t const thread_group_x, uint32_t const thread_group_y, uint32_t const thread_group_z) {
    _impl->dispatch(command_list, thread_group_x, thread_group_y, thread_group_z);
}

void Device::bind_pipeline(Handle<CommandList> const& command_list, Handle<Pipeline> const& pipeline) {
    _impl->bind_pipeline(command_list, pipeline);
}

void Device::bind_resources(Handle<CommandList> const& command_list, Handle<DescriptorLayout> const& descriptor_layout, std::span<DescriptorWriteDesc const> const write_descs) {
    _impl->bind_resources(command_list, descriptor_layout, write_descs);
}

void Device::copy_buffer_region(
    Handle<CommandList> const& command_list, 
    Handle<Buffer> const& dest, 
    uint64_t const dest_offset, 
    Handle<Buffer> const& source, 
    uint64_t const source_offset,
    size_t const size
) {
    _impl->copy_buffer_region(command_list, dest, dest_offset, source, source_offset, size);
}

void Device::copy_texture_region(
    Handle<CommandList> const& command_list,
    Handle<Texture> const& dest,
    Handle<Buffer> const& source,
    std::span<TextureCopyRegion const> const regions
) {
    _impl->copy_texture_region(command_list, dest, source, regions);
}

void Device::bind_vertex_buffer(Handle<CommandList> const& command_list, uint32_t const index, Handle<Buffer> const& buffer, uint64_t const offset) {
    _impl->bind_vertex_buffer(command_list, index, buffer, offset);
}

void Device::bind_index_buffer(Handle<CommandList> const& command_list, Handle<Buffer> const& buffer, uint64_t const offset) {
    _impl->bind_index_buffer(command_list, buffer, offset);
}

void Device::draw(Handle<CommandList> const& command_list, uint32_t const vertex_count, uint32_t const instance_count, uint32_t const vertex_offset) {
    _impl->draw(command_list, vertex_count, instance_count, vertex_offset);
}

void Device::draw_indexed(Handle<CommandList> const& command_list, uint32_t const index_count, uint32_t const instance_count, uint32_t const instance_offset) {
    _impl->draw_indexed(command_list, index_count, instance_count, instance_offset);
}

AdapterDesc Device::adapter_desc() const {
    return _impl->adapter_desc();
}