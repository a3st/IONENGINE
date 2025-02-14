// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "dx12.hpp"
#include "precompiled.h"

namespace ionengine::rhi
{
    auto HRESULT_to_string(HRESULT const hr) -> std::string
    {
        switch (hr)
        {
            case E_FAIL: {
                return "attempted to create a device with the debug layer enabled and the layer is not installed";
            }
            case E_INVALIDARG: {
                return "an invalid parameter was passed to the returning function";
            }
            case E_OUTOFMEMORY: {
                return "Direct3D could not allocate sufficient memory to complete the call";
            }
            case E_NOTIMPL: {
                return "the method call isn't implemented with the passed parameter combination";
            }
            case S_FALSE: {
                return "alternate success value, indicating a successful but nonstandard completion";
            }
            case S_OK: {
                return "no error occurred";
            }
            case D3D12_ERROR_ADAPTER_NOT_FOUND: {
                return "the specified cached PSO was created on a different adapter and cannot be reused on the "
                       "current adapter";
            }
            case D3D12_ERROR_DRIVER_VERSION_MISMATCH: {
                return "the specified cached PSO was created on a different driver version and cannot be reused on the "
                       "current adapter";
            }
            case DXGI_ERROR_INVALID_CALL: {
                return "the method call is invalid. For example, a method's parameter may not be a valid pointer";
            }
            case DXGI_ERROR_WAS_STILL_DRAWING: {
                return "the previous blit operation that is transferring information to or from this surface is "
                       "incomplete";
            }
            default: {
                return "an unknown error has occurred";
            }
        }
    }

    auto throwIfFailed(HRESULT hr) -> void
    {
        if (FAILED(hr))
        {
            throw std::runtime_error(HRESULT_to_string(hr));
        }
    }

    auto DXGI_FORMAT_to_TextureFormat(DXGI_FORMAT const format) -> TextureFormat
    {
        switch (format)
        {
            case DXGI_FORMAT_UNKNOWN:
                return TextureFormat::Unknown;
            case DXGI_FORMAT_R8G8B8A8_UNORM:
                return TextureFormat::RGBA8_UNORM;
            case DXGI_FORMAT_B8G8R8A8_UNORM:
                return TextureFormat::BGRA8_UNORM;
            case DXGI_FORMAT_B8G8R8X8_UNORM:
                return TextureFormat::BGR8_UNORM;
            case DXGI_FORMAT_R8_UNORM:
                return TextureFormat::R8_UNORM;
            case DXGI_FORMAT_BC1_UNORM:
                return TextureFormat::BC1;
            case DXGI_FORMAT_BC3_UNORM:
                return TextureFormat::BC3;
            case DXGI_FORMAT_BC4_UNORM:
                return TextureFormat::BC4;
            case DXGI_FORMAT_BC5_UNORM:
                return TextureFormat::BC5;
            case DXGI_FORMAT_D32_FLOAT:
                return TextureFormat::D32_FLOAT;
            case DXGI_FORMAT_R16G16B16A16_FLOAT:
                return TextureFormat::RGBA16_FLOAT;
            default:
                return TextureFormat::Unknown;
        }
    }

    auto TextureFormat_to_DXGI_FORMAT(TextureFormat const format) -> DXGI_FORMAT
    {
        switch (format)
        {
            case TextureFormat::Unknown:
                return DXGI_FORMAT_UNKNOWN;
            case TextureFormat::RGBA8_UNORM:
                return DXGI_FORMAT_R8G8B8A8_UNORM;
            case TextureFormat::BGRA8_UNORM:
                return DXGI_FORMAT_B8G8R8A8_UNORM;
            case TextureFormat::BGR8_UNORM:
                return DXGI_FORMAT_B8G8R8X8_UNORM;
            case TextureFormat::R8_UNORM:
                return DXGI_FORMAT_R8_UNORM;
            case TextureFormat::BC1:
                return DXGI_FORMAT_BC1_UNORM;
            case TextureFormat::BC3:
                return DXGI_FORMAT_BC3_UNORM;
            case TextureFormat::BC4:
                return DXGI_FORMAT_BC4_UNORM;
            case TextureFormat::BC5:
                return DXGI_FORMAT_BC5_UNORM;
            case TextureFormat::D32_FLOAT:
                return DXGI_FORMAT_D32_FLOAT;
            case TextureFormat::RGBA16_FLOAT:
                return DXGI_FORMAT_R16G16B16A16_FLOAT;
            default:
                return DXGI_FORMAT_UNKNOWN;
        }
    }

    auto TextureDimension_to_D3D12_RESOURCE_DIMENSION(TextureDimension const dimension) -> D3D12_RESOURCE_DIMENSION
    {
        switch (dimension)
        {
            case TextureDimension::_1D:
                return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
            case TextureDimension::Cube:
            case TextureDimension::_2DArray:
            case TextureDimension::CubeArray:
            case TextureDimension::_2D:
                return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            case TextureDimension::_3D:
                return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
            default:
                throw std::invalid_argument("Invalid argument for conversion");
        }
    }

    auto CompareOp_to_D3D12_COMPARISON_FUNC(CompareOp const compareOp) -> D3D12_COMPARISON_FUNC
    {
        switch (compareOp)
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
                throw std::invalid_argument("Invalid argument for conversion");
        }
    }

    auto Blend_to_D3D12_BLEND(Blend const blend) -> D3D12_BLEND
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
                throw std::invalid_argument("Invalid argument for conversion");
        }
    }

    auto BlendOp_to_D3D12_BLEND_OP(BlendOp const blendOp) -> D3D12_BLEND_OP
    {
        switch (blendOp)
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
                throw std::invalid_argument("Invalid argument for conversion");
        }
    }

    auto FillMode_to_D3D12_FILL_MODE(FillMode const fillMode) -> D3D12_FILL_MODE
    {
        switch (fillMode)
        {
            case FillMode::Solid:
                return D3D12_FILL_MODE_SOLID;
            case FillMode::Wireframe:
                return D3D12_FILL_MODE_WIREFRAME;
            default:
                throw std::invalid_argument("Invalid argument for conversion");
        }
    }

    auto CullMode_to_D3D12_CULL_MODE(CullMode const cullMode) -> D3D12_CULL_MODE
    {
        switch (cullMode)
        {
            case CullMode::Back:
                return D3D12_CULL_MODE_BACK;
            case CullMode::Front:
                return D3D12_CULL_MODE_FRONT;
            case CullMode::None:
                return D3D12_CULL_MODE_NONE;
            default:
                throw std::invalid_argument("Invalid argument for conversion");
        }
    }

    auto Filter_to_D3D12_FILTER(Filter const filter) -> D3D12_FILTER
    {
        switch (filter)
        {
            case Filter::MinMagMipLinear:
                return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
            case Filter::ComparisonMinMagMipLinear:
                return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
            case Filter::Anisotropic:
                return D3D12_FILTER_COMPARISON_ANISOTROPIC;
            default:
                throw std::invalid_argument("Invalid argument for conversion");
        }
    }

    auto AddressMode_to_D3D12_TEXTURE_ADDRESS_MODE(AddressMode const addressMode)
    {
        switch (addressMode)
        {
            case AddressMode::Clamp:
                return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
            case AddressMode::Wrap:
                return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            case AddressMode::Mirror:
                return D3D12_TEXTURE_ADDRESS_MODE_MIRROR;
            default:
                throw std::invalid_argument("Invalid argument for conversion");
        }
    };

    auto RenderPassLoadOp_to_D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE(RenderPassLoadOp const loadOp)
        -> D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE
    {
        switch (loadOp)
        {
            case RenderPassLoadOp::Clear:
                return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
            case RenderPassLoadOp::Load:
                return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
            case RenderPassLoadOp::DontCare:
                return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
            default:
                throw std::invalid_argument("Invalid argument for conversion");
        }
    }

    auto RenderPassStoreOp_to_D3D12_RENDER_PASS_ENDING_ACCESS_TYPE(RenderPassStoreOp const storeOp)
        -> D3D12_RENDER_PASS_ENDING_ACCESS_TYPE
    {
        switch (storeOp)
        {
            case RenderPassStoreOp::Store:
                return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
            case RenderPassStoreOp::DontCare:
                return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
            default:
                throw std::invalid_argument("Invalid argument for conversion");
        }
    }

    auto ResourceState_to_D3D12_RESOURCE_STATES(ResourceState const state) -> D3D12_RESOURCE_STATES
    {
        switch (state)
        {
            case ResourceState::Common:
                return D3D12_RESOURCE_STATE_COMMON;
            case ResourceState::DepthStencilRead:
                return D3D12_RESOURCE_STATE_DEPTH_READ;
            case ResourceState::DepthStencilWrite:
                return D3D12_RESOURCE_STATE_DEPTH_WRITE;
            case ResourceState::RenderTarget:
                return D3D12_RESOURCE_STATE_RENDER_TARGET;
            case ResourceState::UnorderedAccess:
                return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            case ResourceState::ShaderRead:
                return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
            case ResourceState::CopySource:
                return D3D12_RESOURCE_STATE_COPY_SOURCE;
            case ResourceState::CopyDest:
                return D3D12_RESOURCE_STATE_COPY_DEST;
            default:
                throw std::invalid_argument("Invalid argument for conversion");
        }
    }

    auto VertexFormat_to_DXGI_FORMAT(VertexFormat const format) -> DXGI_FORMAT
    {
        switch (format)
        {
            case VertexFormat::RGBA32_FLOAT:
                return DXGI_FORMAT_R32G32B32A32_FLOAT;
            case VertexFormat::RGBA32_SINT:
                return DXGI_FORMAT_R32G32B32A32_SINT;
            case VertexFormat::RGBA32_UINT:
                return DXGI_FORMAT_R32G32B32A32_UINT;
            case VertexFormat::RGB32_FLOAT:
                return DXGI_FORMAT_R32G32B32_FLOAT;
            case VertexFormat::RGB32_SINT:
                return DXGI_FORMAT_R32G32B32_SINT;
            case VertexFormat::RGB32_UINT:
                return DXGI_FORMAT_R32G32B32_UINT;
            case VertexFormat::RG32_FLOAT:
                return DXGI_FORMAT_R32G32_FLOAT;
            case VertexFormat::RG32_SINT:
                return DXGI_FORMAT_R32G32_SINT;
            case VertexFormat::RG32_UINT:
                return DXGI_FORMAT_R32G32_UINT;
            case VertexFormat::R32_FLOAT:
                return DXGI_FORMAT_R32_FLOAT;
            case VertexFormat::R32_SINT:
                return DXGI_FORMAT_R32_SINT;
            case VertexFormat::R32_UINT:
                return DXGI_FORMAT_R32_UINT;
            default:
                return DXGI_FORMAT_UNKNOWN;
        }
    }

    HRESULT STDMETHODCALLTYPE DescriptorAllocation::QueryInterface(REFIID riid, void** ppvObject)
    {
        return E_NOINTERFACE;
    }

    ULONG STDMETHODCALLTYPE DescriptorAllocation::AddRef()
    {
        return ++refCount;
    }

    ULONG STDMETHODCALLTYPE DescriptorAllocation::Release()
    {
        const uint32_t newRefCount = --refCount;
        if (newRefCount == 0)
        {
            allocator->deallocate(this);
        }
        return newRefCount;
    }

    auto DescriptorAllocation::initialize(DescriptorAllocator* allocator, ID3D12DescriptorHeap* heap,
                                          D3D12_DESCRIPTOR_HEAP_TYPE const heapType, uint32_t const incrementSize,
                                          uint32_t const offset) -> void
    {
        this->allocator = allocator;
        this->heap = heap;
        this->heapType = heapType;
        this->incrementSize = incrementSize;
        this->offset = offset;
        refCount = 1;
    }

    auto DescriptorAllocation::getCPUHandle() const -> D3D12_CPU_DESCRIPTOR_HANDLE
    {
        return {.ptr = heap->GetCPUDescriptorHandleForHeapStart().ptr + incrementSize * offset};
    }

    auto DescriptorAllocation::getGPUHandle() const -> D3D12_GPU_DESCRIPTOR_HANDLE
    {
        return {.ptr = heap->GetGPUDescriptorHandleForHeapStart().ptr + incrementSize * offset};
    }

    auto DescriptorAllocation::getOffset() const -> uint32_t
    {
        return offset;
    }

    DescriptorAllocator::DescriptorAllocator(ID3D12Device1* device) : device(device)
    {
        std::array<D3D12_DESCRIPTOR_HEAP_TYPE, 4> heapTypes{
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV};
        std::array<uint32_t, 4> descriptorLimits = {std::to_underlying(DescriptorAllocatorLimits::RTV),
                                                    std::to_underlying(DescriptorAllocatorLimits::CBV_SRV_UAV),
                                                    std::to_underlying(DescriptorAllocatorLimits::Sampler),
                                                    std::to_underlying(DescriptorAllocatorLimits::DSV)};

        for (uint32_t const i : std::views::iota(0u, 4u))
        {
            D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{.Type = heapTypes[i],
                                                          .NumDescriptors = descriptorLimits[i],
                                                          .Flags =
                                                              heapTypes[i] == D3D12_DESCRIPTOR_HEAP_TYPE_RTV ||
                                                                      heapTypes[i] == D3D12_DESCRIPTOR_HEAP_TYPE_DSV
                                                                  ? D3D12_DESCRIPTOR_HEAP_FLAG_NONE
                                                                  : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE};
            winrt::com_ptr<ID3D12DescriptorHeap> descriptorHeap;
            throwIfFailed(device->CreateDescriptorHeap(&descriptorHeapDesc, __uuidof(ID3D12DescriptorHeap),
                                                       descriptorHeap.put_void()));

            std::vector<uint8_t> free(descriptorLimits[i]);
            std::fill(free.begin(), free.end(), 0x0);

            auto allocations = std::unique_ptr<DescriptorAllocation[]>(new DescriptorAllocation[descriptorLimits[i]]);

            Chunk chunk{.heap = descriptorHeap,
                        .free = std::move(free),
                        .size = descriptorLimits[i],
                        .incrementSize = device->GetDescriptorHandleIncrementSize(heapTypes[i]),
                        .allocations = std::move(allocations)};
            chunks.emplace(heapTypes[i], std::move(chunk));
        }
    }

    auto DescriptorAllocator::allocate(D3D12_DESCRIPTOR_HEAP_TYPE const heapType, DescriptorAllocation** allocation)
        -> HRESULT
    {
        std::lock_guard lock(mutex);

        Chunk& chunk = chunks[heapType];
        if (1 > chunk.size - chunk.offset)
        {
            return E_OUTOFMEMORY;
        }

        bool success = false;
        uint32_t allocOffset = 0;

        for (uint32_t const i : std::views::iota(chunk.offset, chunk.size))
        {
            if (chunk.free[i] == 0x1)
            {
                continue;
            }
            else
            {
                allocOffset = i;
                success = true;
                break;
            }
        }

        if (success)
        {
            chunk.free[allocOffset] = 0x1;
            chunk.offset = allocOffset + 1;

            chunk.allocations[allocOffset].initialize(this, chunk.heap.get(), heapType, chunk.incrementSize,
                                                      allocOffset);

            (*allocation) = &chunk.allocations[allocOffset];
            return S_OK;
        }
        else
        {
            return E_OUTOFMEMORY;
        }
    }

    auto DescriptorAllocator::deallocate(DescriptorAllocation* allocation) -> void
    {
        std::lock_guard lock(mutex);

        Chunk& chunk = chunks[allocation->heapType];
        chunk.free[allocation->offset] = 0x0;
        chunk.offset = std::min(chunk.offset, allocation->offset);
    }

    auto DescriptorAllocator::getDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE const heapType) -> ID3D12DescriptorHeap*
    {
        return chunks[heapType].heap.get();
    }

    DX12Buffer::DX12Buffer(ID3D12Device1* device, D3D12MA::Allocator* memoryAllocator,
                           DescriptorAllocator* descriptorAllocator, BufferCreateInfo const& createInfo)
        : memoryAllocator(memoryAllocator), descriptorAllocator(descriptorAllocator), size(createInfo.size),
          flags(createInfo.flags)
    {
        D3D12_RESOURCE_DESC resourceDesc{.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER,
                                         .Height = 1,
                                         .DepthOrArraySize = 1,
                                         .MipLevels = 1,
                                         .Format = DXGI_FORMAT_UNKNOWN,
                                         .SampleDesc = {.Count = 1},
                                         .Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR};
        if (flags & BufferUsage::ConstantBuffer)
        {
            uint32_t const constantBufferSizeAlignment = 256;
            resourceDesc.Width = static_cast<uint64_t>(
                (static_cast<uint32_t>(size) + (constantBufferSizeAlignment - 1)) & ~(constantBufferSizeAlignment - 1));
        }
        else
        {
            resourceDesc.Width = size;
        }

        D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;
        D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;

        if (flags & BufferUsage::MapWrite)
        {
            initialState = D3D12_RESOURCE_STATE_GENERIC_READ;
            heapType = D3D12_HEAP_TYPE_UPLOAD;
        }
        else if (flags & BufferUsage::MapRead)
        {
            initialState = D3D12_RESOURCE_STATE_COPY_DEST;
            heapType = D3D12_HEAP_TYPE_READBACK;
        }

        D3D12MA::ALLOCATION_DESC const maAllocationDesc{.HeapType = heapType};
        throwIfFailed(memoryAllocator->CreateResource(&maAllocationDesc, &resourceDesc, initialState, nullptr,
                                                      memoryAllocation.put(), __uuidof(ID3D12Resource),
                                                      resource.put_void()));

        if (flags & BufferUsage::ConstantBuffer)
        {
            assert(descriptorAllocator && "To create a buffer with views, you need to pass the allocator descriptor");

            winrt::com_ptr<DescriptorAllocation> descriptorAllocation;
            throwIfFailed(
                descriptorAllocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, descriptorAllocation.put()));
            descriptorAllocations.emplace(BufferUsage::ConstantBuffer, descriptorAllocation);

            D3D12_CONSTANT_BUFFER_VIEW_DESC const constantBufferViewDesc{
                .BufferLocation = resource->GetGPUVirtualAddress(),
                .SizeInBytes = static_cast<uint32_t>(resourceDesc.Width)};
            device->CreateConstantBufferView(&constantBufferViewDesc, descriptorAllocation->getCPUHandle());
        }
        if (flags & BufferUsage::ShaderResource)
        {
            assert(descriptorAllocator && "To create a buffer with views, you need to pass the allocator descriptor");

            winrt::com_ptr<DescriptorAllocation> descriptorAllocation;
            throwIfFailed(
                descriptorAllocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, descriptorAllocation.put()));
            descriptorAllocations.emplace(BufferUsage::ShaderResource, descriptorAllocation);

            D3D12_SHADER_RESOURCE_VIEW_DESC const shaderResourceViewDesc{
                .Format = DXGI_FORMAT_UNKNOWN,
                .ViewDimension = D3D12_SRV_DIMENSION_BUFFER,
                .Buffer = {.FirstElement = 0,
                           .NumElements = static_cast<uint32_t>(size / createInfo.elementStride),
                           .StructureByteStride = createInfo.elementStride}};
            device->CreateShaderResourceView(resource.get(), &shaderResourceViewDesc,
                                             descriptorAllocation->getCPUHandle());
        }
        if (flags & BufferUsage::UnorderedAccess)
        {
            assert(descriptorAllocator && "To create a buffer with views, you need to pass the allocator descriptor");

            winrt::com_ptr<DescriptorAllocation> descriptorAllocation;
            throwIfFailed(
                descriptorAllocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, descriptorAllocation.put()));
            descriptorAllocations.emplace(BufferUsage::UnorderedAccess, descriptorAllocation);

            D3D12_UNORDERED_ACCESS_VIEW_DESC const unorderedAccessViewDesc{
                .Format = DXGI_FORMAT_UNKNOWN,
                .ViewDimension = D3D12_UAV_DIMENSION_BUFFER,
                .Buffer = {.FirstElement = 0,
                           .NumElements = static_cast<uint32_t>(size / createInfo.elementStride),
                           .StructureByteStride = createInfo.elementStride}};
            device->CreateUnorderedAccessView(resource.get(), nullptr, &unorderedAccessViewDesc,
                                              descriptorAllocation->getCPUHandle());
        }
    }

    auto DX12Buffer::mapMemory() -> uint8_t*
    {
        uint8_t* mappedBytes;
        if (flags & BufferUsage::MapWrite)
        {
            throwIfFailed(resource->Map(0, nullptr, reinterpret_cast<void**>(&mappedBytes)));
        }
        else
        {
            D3D12_RANGE const range{.Begin = 0, .End = size};
            throwIfFailed(resource->Map(0, &range, reinterpret_cast<void**>(&mappedBytes)));
        }
        return mappedBytes;
    }

    auto DX12Buffer::unmapMemory() -> void
    {
        if (flags & BufferUsage::MapWrite)
        {
            D3D12_RANGE const range{.Begin = 0, .End = size};
            resource->Unmap(0, &range);
        }
        else if (flags & BufferUsage::MapRead)
        {
            resource->Unmap(0, nullptr);
        }
    }

    auto DX12Buffer::getSize() const -> size_t
    {
        return size;
    }

    auto DX12Buffer::getFlags() const -> BufferUsageFlags
    {
        return flags;
    }

    auto DX12Buffer::getDescriptorOffset(BufferUsage const usage) const -> uint32_t
    {
        return descriptorAllocations.at(usage)->getOffset();
    }

    auto DX12Buffer::getResource() -> ID3D12Resource*
    {
        return resource.get();
    }

    auto DX12Buffer::getDescriptor(BufferUsage const usage) const -> DescriptorAllocation*
    {
        return descriptorAllocations.at(usage).get();
    }

    DX12Texture::DX12Texture(ID3D12Device1* device, D3D12MA::Allocator* memoryAllocator,
                             DescriptorAllocator* descriptorAllocator, TextureCreateInfo const& createInfo)
        : memoryAllocator(memoryAllocator), descriptorAllocator(descriptorAllocator), width(createInfo.width),
          height(createInfo.height), depth(createInfo.depth), mipLevels(createInfo.mipLevels),
          format(createInfo.format), dimension(createInfo.dimension), flags(createInfo.flags)
    {
        D3D12_RESOURCE_DESC resourceDesc{.Dimension =
                                             TextureDimension_to_D3D12_RESOURCE_DIMENSION(createInfo.dimension),
                                         .Width = width,
                                         .Height = height,
                                         .DepthOrArraySize = static_cast<uint16_t>(depth),
                                         .MipLevels = static_cast<uint16_t>(mipLevels),
                                         .Format = TextureFormat_to_DXGI_FORMAT(format),
                                         .SampleDesc = {.Count = 1},
                                         .Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN};

        D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;
        D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;

        if (flags & TextureUsage::DepthStencil)
        {
            resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            initialState = D3D12_RESOURCE_STATE_DEPTH_READ;
        }
        if (flags & TextureUsage::RenderTarget)
        {
            resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        }
        if (flags & TextureUsage::UnorderedAccess)
        {
            resourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }

        D3D12MA::ALLOCATION_DESC const maAllocationDesc{.HeapType = heapType};
        throwIfFailed(memoryAllocator->CreateResource(&maAllocationDesc, &resourceDesc, initialState, nullptr,
                                                      memoryAllocation.put(), __uuidof(ID3D12Resource),
                                                      resource.put_void()));

        if (flags & TextureUsage::RenderTarget)
        {
            assert(descriptorAllocator && "To create a texture with views, you need to pass the allocator descriptor");

            winrt::com_ptr<DescriptorAllocation> descriptorAllocation;
            throwIfFailed(descriptorAllocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, descriptorAllocation.put()));
            descriptorAllocations.emplace(TextureUsage::RenderTarget, descriptorAllocation);

            D3D12_RENDER_TARGET_VIEW_DESC const renderTargetViewDesc{.Format = resourceDesc.Format,
                                                                     .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D};
            device->CreateRenderTargetView(resource.get(), &renderTargetViewDesc, descriptorAllocation->getCPUHandle());
        }

        if (flags & TextureUsage::DepthStencil)
        {
            assert(descriptorAllocator && "To create a texture with views, you need to pass the allocator descriptor");

            winrt::com_ptr<DescriptorAllocation> descriptorAllocation;
            throwIfFailed(descriptorAllocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, descriptorAllocation.put()));
            descriptorAllocations.emplace(TextureUsage::DepthStencil, descriptorAllocation);

            D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{.Format = DXGI_FORMAT_D32_FLOAT};
            switch (dimension)
            {
                case TextureDimension::_2D: {
                    depthStencilViewDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                    break;
                }
                default: {
                    assert(false && "Creation of depth stencil only possible in 2D dimension");
                    break;
                }
            }
            device->CreateDepthStencilView(resource.get(), &depthStencilViewDesc, descriptorAllocation->getCPUHandle());
        }

        if (flags & TextureUsage::ShaderResource)
        {
            assert(descriptorAllocator && "To create a texture with views, you need to pass the allocator descriptor");

            winrt::com_ptr<DescriptorAllocation> descriptorAllocation;
            throwIfFailed(
                descriptorAllocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, descriptorAllocation.put()));
            descriptorAllocations.emplace(TextureUsage::ShaderResource, descriptorAllocation);

            D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{
                .Format = resourceDesc.Format, .Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING};
            switch (dimension)
            {
                case TextureDimension::_1D: {
                    shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
                    shaderResourceViewDesc.Texture1D.MipLevels = mipLevels;
                    break;
                }
                case TextureDimension::_2D: {
                    shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    shaderResourceViewDesc.Texture2D.MipLevels = mipLevels;
                    break;
                }
                case TextureDimension::_3D: {
                    shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
                    shaderResourceViewDesc.Texture3D.MipLevels = mipLevels;
                    break;
                }
                case TextureDimension::Cube: {
                    shaderResourceViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                    shaderResourceViewDesc.Texture3D.MipLevels = mipLevels;
                    break;
                }
            }
            device->CreateShaderResourceView(resource.get(), &shaderResourceViewDesc,
                                             descriptorAllocation->getCPUHandle());
        }
    }

    DX12Texture::DX12Texture(ID3D12Device1* device, DescriptorAllocator* descriptorAllocator,
                             winrt::com_ptr<ID3D12Resource> resource)
        : memoryAllocator(nullptr), descriptorAllocator(descriptorAllocator), resource(resource),
          flags((TextureUsageFlags)TextureUsage::RenderTarget)
    {
        D3D12_RESOURCE_DESC resourceDesc = resource->GetDesc();
        width = static_cast<uint32_t>(resourceDesc.Width);
        height = resourceDesc.Height;
        depth = resourceDesc.DepthOrArraySize;
        mipLevels = resourceDesc.MipLevels;
        format = DXGI_FORMAT_to_TextureFormat(resourceDesc.Format);

        if (flags & TextureUsage::RenderTarget)
        {
            assert(descriptorAllocator && "to create a texture with views, you need to pass an allocator descriptor");

            winrt::com_ptr<DescriptorAllocation> descriptorAllocation;
            throwIfFailed(descriptorAllocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, descriptorAllocation.put()));
            descriptorAllocations.emplace(TextureUsage::RenderTarget, descriptorAllocation);

            D3D12_RENDER_TARGET_VIEW_DESC const renderTargetViewDesc{.Format = resourceDesc.Format,
                                                                     .ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D};
            device->CreateRenderTargetView(resource.get(), &renderTargetViewDesc, descriptorAllocation->getCPUHandle());
        }
    }

    auto DX12Texture::getWidth() const -> uint32_t
    {
        return width;
    }

    auto DX12Texture::getHeight() const -> uint32_t
    {
        return height;
    }

    auto DX12Texture::getDepth() const -> uint32_t
    {
        return depth;
    }

    auto DX12Texture::getMipLevels() const -> uint32_t
    {
        return mipLevels;
    }

    auto DX12Texture::getFormat() const -> TextureFormat
    {
        return format;
    }

    auto DX12Texture::getFlags() const -> TextureUsageFlags
    {
        return flags;
    }

    auto DX12Texture::getDescriptorOffset(TextureUsage const usage) const -> uint32_t
    {
        return descriptorAllocations.at(usage)->getOffset();
    }

    auto DX12Texture::getResource() -> ID3D12Resource*
    {
        return resource.get();
    }

    auto DX12Texture::getDescriptor(TextureUsage const usage) const -> DescriptorAllocation*
    {
        return descriptorAllocations.at(usage).get();
    }

    DX12Sampler::DX12Sampler(ID3D12Device1* device, DescriptorAllocator* descriptorAllocator,
                             SamplerCreateInfo const& createInfo)
    {
        D3D12_SAMPLER_DESC const samplerDesc{.Filter = Filter_to_D3D12_FILTER(createInfo.filter),
                                             .AddressU = AddressMode_to_D3D12_TEXTURE_ADDRESS_MODE(createInfo.addressU),
                                             .AddressV = AddressMode_to_D3D12_TEXTURE_ADDRESS_MODE(createInfo.addressV),
                                             .AddressW = AddressMode_to_D3D12_TEXTURE_ADDRESS_MODE(createInfo.addressV),
                                             .MaxAnisotropy = createInfo.anisotropic,
                                             .ComparisonFunc =
                                                 CompareOp_to_D3D12_COMPARISON_FUNC(createInfo.compareOp)};

        assert(descriptorAllocator && "to create a sampler, you need to pass an allocator descriptor");

        throwIfFailed(descriptorAllocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, descriptorAllocation.put()));

        device->CreateSampler(&samplerDesc, descriptorAllocation->getCPUHandle());
    }

    auto DX12Sampler::getDescriptorOffset() const -> uint32_t
    {
        return descriptorAllocation->getOffset();
    }

    DX12VertexInput::DX12VertexInput(std::span<VertexDeclarationInfo const> const vertexDeclarations)
    {
        uint32_t offset = 0;

        for (auto const& vertexDeclaration : vertexDeclarations)
        {
            D3D12_INPUT_ELEMENT_DESC inputElementDesc{};
            uint32_t semanticIndex;
            if (std::isdigit(vertexDeclaration.semantic.back()) == 0)
            {
                semanticNames.emplace_back(vertexDeclaration.semantic);
                semanticIndex = 0;
            }
            else
            {
                std::string const semanticName =
                    vertexDeclaration.semantic.substr(0, vertexDeclaration.semantic.size() - 1);
                semanticNames.emplace_back(std::move(semanticName));
                semanticIndex = static_cast<int32_t>(vertexDeclaration.semantic.back()) - 48;
            }

            inputElementDesc.SemanticName = semanticNames.back().c_str();
            inputElementDesc.SemanticIndex = semanticIndex;
            inputElementDesc.Format = VertexFormat_to_DXGI_FORMAT(vertexDeclaration.format);
            inputElementDesc.InputSlot = 0;
            inputElementDesc.AlignedByteOffset = offset;
            inputElementDesc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            inputElementDesc.InstanceDataStepRate = 0;

            inputElementDescs.emplace_back(inputElementDesc);
            offset += sizeof_VertexFormat(vertexDeclaration.format);
        }

        inputSize = offset;
    }

    auto DX12VertexInput::getInputElementDescs() const -> std::span<D3D12_INPUT_ELEMENT_DESC const>
    {
        return inputElementDescs;
    }

    auto DX12VertexInput::getInputSize() const -> uint32_t
    {
        return inputSize;
    }

    DX12Shader::DX12Shader(ID3D12Device4* device, ShaderCreateInfo const& createInfo)
    {
        if (createInfo.pipelineType == rhi::PipelineType::Graphics)
        {
            XXH64_state_t* hasher = ::XXH64_createState();
            {
                DX12ShaderStage shaderStage;
                shaderStage.buffer =
                    createInfo.graphics.vertexStage.shaderCode | std::ranges::to<std::vector<uint8_t>>();
                shaderStage.shaderByteCode = {.pShaderBytecode = shaderStage.buffer.data(),
                                              .BytecodeLength = shaderStage.buffer.size()};
                stages.emplace(DX12ShaderStageType::Vertex, std::move(shaderStage));

                ::XXH64_update(hasher, shaderStage.buffer.data(), shaderStage.buffer.size());
            }

            {
                DX12ShaderStage shaderStage;
                shaderStage.buffer =
                    createInfo.graphics.pixelStage.shaderCode | std::ranges::to<std::vector<uint8_t>>();
                shaderStage.shaderByteCode = {.pShaderBytecode = shaderStage.buffer.data(),
                                              .BytecodeLength = shaderStage.buffer.size()};
                stages.emplace(DX12ShaderStageType::Pixel, std::move(shaderStage));

                ::XXH64_update(hasher, shaderStage.buffer.data(), shaderStage.buffer.size());
            }
            hash = ::XXH64_digest(hasher);

            vertexInput.emplace(createInfo.graphics.vertexDeclarations);
        }
        else
        {
            XXH64_state_t* hasher = ::XXH64_createState();
            {
                DX12ShaderStage shaderStage;
                shaderStage.buffer = createInfo.compute.shaderCode | std::ranges::to<std::vector<uint8_t>>();
                shaderStage.shaderByteCode = {.pShaderBytecode = shaderStage.buffer.data(),
                                              .BytecodeLength = shaderStage.buffer.size()};
                stages.emplace(DX12ShaderStageType::Compute, std::move(shaderStage));

                ::XXH64_update(hasher, shaderStage.buffer.data(), shaderStage.buffer.size());
            }
            hash = ::XXH64_digest(hasher);
        }
    }

    auto DX12Shader::getStages() const -> std::unordered_map<DX12ShaderStageType, DX12ShaderStage> const&
    {
        return stages;
    }

    auto DX12Shader::getPipelineType() const -> PipelineType
    {
        return pipelineType;
    }

    auto DX12Shader::getHash() const -> uint64_t
    {
        return hash;
    }

    auto DX12Shader::getVertexInput() const -> std::optional<DX12VertexInput>
    {
        return vertexInput;
    }

    Pipeline::Pipeline(ID3D12Device4* device, ID3D12RootSignature* rootSignature, DX12Shader* shader,
                       RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                       std::optional<DepthStencilStageInfo> const depthStencil,
                       std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> const& renderTargetFormats,
                       DXGI_FORMAT const depthStencilFormat, ID3DBlob* blob)
        : rootSignature(rootSignature)
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
        graphicsPipelineStateDesc.pRootSignature = rootSignature;

        for (auto const& [stage, stageData] : shader->getStages())
        {
            switch (stage)
            {
                case DX12ShaderStageType::Vertex: {
                    graphicsPipelineStateDesc.VS = stageData.shaderByteCode;

                    D3D12_INPUT_LAYOUT_DESC const inputLayoutDesc{
                        .pInputElementDescs = shader->getVertexInput().value().getInputElementDescs().data(),
                        .NumElements =
                            static_cast<uint32_t>(shader->getVertexInput().value().getInputElementDescs().size())};

                    graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
                    break;
                }
                case DX12ShaderStageType::Pixel: {
                    graphicsPipelineStateDesc.PS = stageData.shaderByteCode;

                    D3D12_RENDER_TARGET_BLEND_DESC const renderTargetBlendDesc{
                        .BlendEnable = blendColor.blendEnable,
                        .SrcBlend = Blend_to_D3D12_BLEND(blendColor.blendSrc),
                        .DestBlend = Blend_to_D3D12_BLEND(blendColor.blendDst),
                        .BlendOp = BlendOp_to_D3D12_BLEND_OP(blendColor.blendOp),
                        .SrcBlendAlpha = Blend_to_D3D12_BLEND(blendColor.blendSrcAlpha),
                        .DestBlendAlpha = Blend_to_D3D12_BLEND(blendColor.blendDstAlpha),
                        .BlendOpAlpha = BlendOp_to_D3D12_BLEND_OP(blendColor.blendOpAlpha),
                        .RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL};

                    D3D12_BLEND_DESC d3d12BlendDesc{};
                    for (uint32_t const i : std::views::iota(0u, renderTargetFormats.size()))
                    {
                        if (renderTargetFormats[i] == DXGI_FORMAT_UNKNOWN)
                        {
                            break;
                        }

                        graphicsPipelineStateDesc.RTVFormats[i] = renderTargetFormats[i];
                        d3d12BlendDesc.RenderTarget[i] = renderTargetBlendDesc;
                    }
                    graphicsPipelineStateDesc.NumRenderTargets = static_cast<uint32_t>(renderTargetFormats.size());
                    graphicsPipelineStateDesc.BlendState = d3d12BlendDesc;
                    graphicsPipelineStateDesc.DSVFormat = depthStencilFormat;
                    break;
                }
            }
        }

        D3D12_RASTERIZER_DESC const rasterizerDesc{.FillMode = FillMode_to_D3D12_FILL_MODE(rasterizer.fillMode),
                                                   .CullMode = CullMode_to_D3D12_CULL_MODE(rasterizer.cullMode),
                                                   .FrontCounterClockwise = true,
                                                   .DepthBias = D3D12_DEFAULT_DEPTH_BIAS,
                                                   .DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP,
                                                   .SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS,
                                                   .DepthClipEnable = true,
                                                   .MultisampleEnable = false,
                                                   .AntialiasedLineEnable = false,
                                                   .ForcedSampleCount = 0,
                                                   .ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF};

        graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

        graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        graphicsPipelineStateDesc.SampleMask = std::numeric_limits<uint32_t>::max();
        graphicsPipelineStateDesc.SampleDesc.Count = 1;

        D3D12_DEPTH_STENCILOP_DESC const depthStencilOpDesc{.StencilFailOp = D3D12_STENCIL_OP_KEEP,
                                                            .StencilDepthFailOp = D3D12_STENCIL_OP_KEEP,
                                                            .StencilPassOp = D3D12_STENCIL_OP_KEEP,
                                                            .StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS};

        auto depthStencilValue = depthStencil.value_or(DepthStencilStageInfo::Default());
        D3D12_DEPTH_STENCIL_DESC const depthStencilDesc{
            .DepthEnable = depthStencilValue.depthWrite,
            .DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL,
            .DepthFunc = CompareOp_to_D3D12_COMPARISON_FUNC(depthStencilValue.depthFunc),
            .StencilEnable = depthStencilValue.stencilWrite,
            .StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK,
            .StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK,
            .FrontFace = depthStencilOpDesc,
            .BackFace = depthStencilOpDesc};

        graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;

        if (blob)
        {
            graphicsPipelineStateDesc.CachedPSO.pCachedBlob = blob->GetBufferPointer();
            graphicsPipelineStateDesc.CachedPSO.CachedBlobSizeInBytes = blob->GetBufferSize();
        }

        HRESULT result = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, __uuidof(ID3D12PipelineState),
                                                             pipelineState.put_void());
        if (result == D3D12_ERROR_ADAPTER_NOT_FOUND | result == D3D12_ERROR_DRIVER_VERSION_MISMATCH |
            result == E_INVALIDARG)
        {
            graphicsPipelineStateDesc.CachedPSO = {};
            throwIfFailed(device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc, __uuidof(ID3D12PipelineState),
                                                              pipelineState.put_void()));
        }
        else
        {
            throwIfFailed(result);
        }
    }

    auto Pipeline::getPipelineState() -> ID3D12PipelineState*
    {
        return pipelineState.get();
    }

    auto Pipeline::getRootSignature() -> ID3D12RootSignature*
    {
        return rootSignature;
    }

    PipelineCache::PipelineCache(ID3D12Device4* device, RHICreateInfo const& createInfo) : device(device)
    {
        D3D12_ROOT_PARAMETER1 const rootParameter{
            .ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS,
            .Constants = {.ShaderRegister = 0, .RegisterSpace = 0, .Num32BitValues = 16},
            .ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL};

        D3D12_VERSIONED_ROOT_SIGNATURE_DESC const rootSignatureDesc{
            .Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
            .Desc_1_1 = {.NumParameters = 1,
                         .pParameters = &rootParameter,
                         .Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                                  D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
                                  D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED}};

        winrt::com_ptr<ID3DBlob> blob;
        throwIfFailed(::D3D12SerializeVersionedRootSignature(&rootSignatureDesc, blob.put(), nullptr));
        throwIfFailed(device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
                                                  __uuidof(ID3D12RootSignature), rootSignature.put_void()));
    }

    auto PipelineCache::get(DX12Shader* shader, RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                            std::optional<DepthStencilStageInfo> const depthStencil,
                            std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> const& renderTargetFormats,
                            DXGI_FORMAT const depthStencilFormat) -> core::ref_ptr<Pipeline>
    {
        Entry entry{.shaderHash = shader->getHash(),
                    .rasterizer = rasterizer,
                    .blendColor = blendColor,
                    .depthStencil = depthStencil,
                    .renderTargetFormats = renderTargetFormats,
                    .depthStencilFormat = depthStencilFormat};

        std::unique_lock lock(mutex);
        auto result = entries.find(entry);
        if (result != entries.end())
        {
            return result->second;
        }
        else
        {
            lock.unlock();
            auto pipeline = core::make_ref<Pipeline>(device, rootSignature.get(), shader, rasterizer, blendColor,
                                                     depthStencil, renderTargetFormats, depthStencilFormat, nullptr);

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

    DX12FutureImpl::DX12FutureImpl(ID3D12CommandQueue* queue, ID3D12Fence* fence, HANDLE fenceEvent,
                                   uint64_t const fenceValue)
        : queue(queue), fence(fence), fenceEvent(fenceEvent), fenceValue(fenceValue)
    {
    }

    auto DX12FutureImpl::getResult() const -> bool
    {
        return fence->GetCompletedValue() >= fenceValue;
    }

    auto DX12FutureImpl::wait() -> void
    {
        throwIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
        ::WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
    }

    auto DX12DeviceContext_barrier(ID3D12GraphicsCommandList4* commandList, core::ref_ptr<Buffer> dest,
                                   ResourceState const before, ResourceState const after) -> void
    {
        auto dxDestBuffer = dynamic_cast<DX12Buffer*>(dest.get());

        D3D12_RESOURCE_BARRIER const resourceBarrier{.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                                                     .Transition = {
                                                         .pResource = dxDestBuffer->getResource(),
                                                         .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                                                         .StateBefore = ResourceState_to_D3D12_RESOURCE_STATES(before),
                                                         .StateAfter = ResourceState_to_D3D12_RESOURCE_STATES(after),
                                                     }};
        commandList->ResourceBarrier(1, &resourceBarrier);
    }

    auto DX12DeviceContext_barrier(ID3D12GraphicsCommandList4* commandList, core::ref_ptr<Texture> dest,
                                   ResourceState const before, ResourceState const after) -> void
    {
        auto dxDestTexture = dynamic_cast<DX12Texture*>(dest.get());

        D3D12_RESOURCE_BARRIER const resourceBarrier{.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION,
                                                     .Transition = {
                                                         .pResource = dxDestTexture->getResource(),
                                                         .Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
                                                         .StateBefore = ResourceState_to_D3D12_RESOURCE_STATES(before),
                                                         .StateAfter = ResourceState_to_D3D12_RESOURCE_STATES(after),
                                                     }};
        commandList->ResourceBarrier(1, &resourceBarrier);
    }

    DX12GraphicsContext::DX12GraphicsContext(ID3D12Device4* device, PipelineCache* pipelineCache,
                                             DescriptorAllocator* descriptorAllocator, DeviceQueueData& deviceQueue,
                                             HANDLE fenceEvent)
        : device(device), pipelineCache(pipelineCache), descriptorAllocator(descriptorAllocator),
          deviceQueue(&deviceQueue), fenceEvent(fenceEvent), isCommandListOpened(false)
    {
        throwIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator),
                                                     commandAllocator.put_void()));

        throwIfFailed(device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE,
                                                 __uuidof(ID3D12GraphicsCommandList4), commandList.put_void()));
    }

    auto DX12GraphicsContext::tryResetCommandList() -> void
    {
        if (isCommandListOpened)
        {
            return;
        }

        throwIfFailed(commandAllocator->Reset());
        throwIfFailed(commandList->Reset(commandAllocator.get(), nullptr));

        std::array<ID3D12DescriptorHeap*, 2> const descriptorHeaps{
            descriptorAllocator->getDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
            descriptorAllocator->getDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)};
        commandList->SetDescriptorHeaps(static_cast<uint32_t>(descriptorHeaps.size()), descriptorHeaps.data());

        currentShader = nullptr;

        isCommandListOpened = true;
    }

    auto DX12GraphicsContext::setGraphicsPipelineOptions(core::ref_ptr<Shader> shader,
                                                         RasterizerStageInfo const& rasterizer,
                                                         BlendColorInfo const& blendColor,
                                                         std::optional<DepthStencilStageInfo> const depthStencil)
        -> void
    {
        this->tryResetCommandList();

        auto dxShader = dynamic_cast<DX12Shader*>(shader.get());
        currentShader = shader;

        auto pipeline =
            pipelineCache->get(dxShader, rasterizer, blendColor, depthStencil, renderTargetFormats, depthStencilFormat);

        commandList->SetGraphicsRootSignature(pipeline->getRootSignature());
        commandList->SetPipelineState(pipeline->getPipelineState());
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    auto DX12GraphicsContext::bindDescriptor(uint32_t const index, uint32_t const descriptor) -> void
    {
        bindings[index] = descriptor;
    }

    auto DX12GraphicsContext::beginRenderPass(std::span<RenderPassColorInfo> const colors,
                                              std::optional<RenderPassDepthStencilInfo> depthStencil) -> void
    {
        this->tryResetCommandList();

        renderTargetFormats.fill(DXGI_FORMAT_UNKNOWN);
        depthStencilFormat = DXGI_FORMAT_UNKNOWN;

        std::array<D3D12_RENDER_PASS_RENDER_TARGET_DESC, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT>
            renderPassRenderTargets;

        for (uint32_t const i : std::views::iota(0u, colors.size()))
        {
            D3D12_RENDER_PASS_BEGINNING_ACCESS const begin{
                .Type = RenderPassLoadOp_to_D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE(colors[i].loadOp),
                .Clear = {.ClearValue = {.Format = TextureFormat_to_DXGI_FORMAT(colors[i].texture->getFormat()),
                                         .Color = {colors[i].clearColor.r, colors[i].clearColor.g,
                                                   colors[i].clearColor.b, colors[i].clearColor.a}}}};
            D3D12_RENDER_PASS_ENDING_ACCESS const end{
                .Type = RenderPassStoreOp_to_D3D12_RENDER_PASS_ENDING_ACCESS_TYPE(colors[i].storeOp)};

            auto dxTargetTexture = dynamic_cast<DX12Texture*>(colors[i].texture.get());

            renderPassRenderTargets[i].BeginningAccess = begin;
            renderPassRenderTargets[i].EndingAccess = end;
            renderPassRenderTargets[i].cpuDescriptor =
                dxTargetTexture->getDescriptor(TextureUsage::RenderTarget)->getCPUHandle();
            renderTargetFormats[i] = begin.Clear.ClearValue.Format;
        }

        if (depthStencil.has_value())
        {
            auto value = depthStencil.value();
            depthStencilFormat = TextureFormat_to_DXGI_FORMAT(value.texture->getFormat());

            D3D12_RENDER_PASS_DEPTH_STENCIL_DESC renderPassDepthStencil{};

            {
                D3D12_RENDER_PASS_BEGINNING_ACCESS const begin{
                    .Type = RenderPassLoadOp_to_D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE(value.depthLoadOp),
                    .Clear = {
                        .ClearValue = {.Format = depthStencilFormat, .DepthStencil = {.Depth = value.clearDepth}}}};

                D3D12_RENDER_PASS_ENDING_ACCESS const end{
                    .Type = RenderPassStoreOp_to_D3D12_RENDER_PASS_ENDING_ACCESS_TYPE(value.depthStoreOp)};

                renderPassDepthStencil.DepthBeginningAccess = begin;
                renderPassDepthStencil.DepthEndingAccess = end;
            }

            {
                D3D12_RENDER_PASS_BEGINNING_ACCESS const begin{
                    .Type = RenderPassLoadOp_to_D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE(value.stencilLoadOp),
                    .Clear = {
                        .ClearValue = {.Format = depthStencilFormat, .DepthStencil = {.Stencil = value.clearStencil}}}};
                D3D12_RENDER_PASS_ENDING_ACCESS const end{
                    .Type = RenderPassStoreOp_to_D3D12_RENDER_PASS_ENDING_ACCESS_TYPE(value.stencilStoreOp)};

                renderPassDepthStencil.StencilBeginningAccess = begin;
                renderPassDepthStencil.StencilEndingAccess = end;
            }

            auto dxTargetTexture = dynamic_cast<DX12Texture*>(value.texture.get());

            renderPassDepthStencil.cpuDescriptor =
                dxTargetTexture->getDescriptor(TextureUsage::DepthStencil)->getCPUHandle();

            commandList->BeginRenderPass(static_cast<uint32_t>(colors.size()), renderPassRenderTargets.data(),
                                         &renderPassDepthStencil, D3D12_RENDER_PASS_FLAG_NONE);
        }
        else
        {
            commandList->BeginRenderPass(static_cast<uint32_t>(colors.size()), renderPassRenderTargets.data(), nullptr,
                                         D3D12_RENDER_PASS_FLAG_NONE);
        }
    }

    auto DX12GraphicsContext::endRenderPass() -> void
    {
        commandList->EndRenderPass();
    }

    auto DX12GraphicsContext::bindVertexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size)
        -> void
    {
        this->tryResetCommandList();

        auto dxBuffer = dynamic_cast<DX12Buffer*>(buffer.get());

        D3D12_VERTEX_BUFFER_VIEW const vertexBufferView{
            .BufferLocation = dxBuffer->getResource()->GetGPUVirtualAddress() + offset,
            .SizeInBytes = static_cast<uint32_t>(size),
            .StrideInBytes = currentShader->getVertexInput().value().getInputSize()};
        commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    }

    auto DX12GraphicsContext::bindIndexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                                              IndexFormat const format) -> void
    {
        this->tryResetCommandList();

        auto dxBuffer = dynamic_cast<DX12Buffer*>(buffer.get());

        D3D12_INDEX_BUFFER_VIEW indexBufferView{.BufferLocation =
                                                    dxBuffer->getResource()->GetGPUVirtualAddress() + offset};
        switch (format)
        {
            case IndexFormat::Uint32: {
                indexBufferView.Format = DXGI_FORMAT_R32_UINT;
                break;
            }
            case IndexFormat::Uint16: {
                indexBufferView.Format = DXGI_FORMAT_R16_UINT;
                break;
            }
        }
        indexBufferView.SizeInBytes = size;

        commandList->IASetIndexBuffer(&indexBufferView);
    }

    auto DX12GraphicsContext::drawIndexed(uint32_t const indexCount, uint32_t const instanceCount) -> void
    {
        commandList->SetGraphicsRoot32BitConstants(0, static_cast<uint32_t>(bindings.size()), bindings.data(), 0);
        commandList->DrawIndexedInstanced(indexCount, instanceCount, 0, 0, 0);
    }

    auto DX12GraphicsContext::draw(uint32_t const vertexCount, uint32_t const instanceCount) -> void
    {
        commandList->SetGraphicsRoot32BitConstants(0, static_cast<uint32_t>(bindings.size()), bindings.data(), 0);
        commandList->DrawInstanced(vertexCount, instanceCount, 0, 0);
    }

    auto DX12GraphicsContext::setViewport(int32_t const x, int32_t const y, uint32_t const width, uint32_t const height)
        -> void
    {
        this->tryResetCommandList();

        D3D12_VIEWPORT const viewport{.TopLeftX = static_cast<float>(x),
                                      .TopLeftY = static_cast<float>(y),
                                      .Width = static_cast<float>(width),
                                      .Height = static_cast<float>(height),
                                      .MinDepth = D3D12_MIN_DEPTH,
                                      .MaxDepth = D3D12_MAX_DEPTH};
        commandList->RSSetViewports(1, &viewport);
    }

    auto DX12GraphicsContext::setScissor(int32_t const left, int32_t const top, int32_t const right,
                                         int32_t const bottom) -> void
    {
        this->tryResetCommandList();

        D3D12_RECT const rect{.left = static_cast<LONG>(left),
                              .top = static_cast<LONG>(top),
                              .right = static_cast<LONG>(right),
                              .bottom = static_cast<LONG>(bottom)};
        commandList->RSSetScissorRects(1, &rect);
    }

    auto DX12GraphicsContext::barrier(core::ref_ptr<Buffer> dest, ResourceState const before, ResourceState const after)
        -> void
    {
        this->tryResetCommandList();

        DX12DeviceContext_barrier(commandList.get(), dest, before, after);
    }

    auto DX12GraphicsContext::barrier(core::ref_ptr<Texture> dest, ResourceState const before,
                                      ResourceState const after) -> void
    {
        this->tryResetCommandList();

        DX12DeviceContext_barrier(commandList.get(), dest, before, after);
    }

    auto DX12GraphicsContext::execute() -> Future<void>
    {
        throwIfFailed(commandList->Close());
        isCommandListOpened = false;

        std::array<ID3D12CommandList*, 1> const commandLists{commandList.get()};
        deviceQueue->queue->ExecuteCommandLists(static_cast<uint32_t>(commandLists.size()), commandLists.data());

        deviceQueue->fenceValue++;
        throwIfFailed(deviceQueue->queue->Signal(deviceQueue->fence.get(), deviceQueue->fenceValue));

        auto futureImpl = std::make_unique<DX12FutureImpl>(deviceQueue->queue.get(), deviceQueue->fence.get(),
                                                           fenceEvent, deviceQueue->fenceValue);
        return Future<void>(std::move(futureImpl));
    }

    auto DX12CopyContext::getSurfaceData(rhi::TextureFormat const format, uint32_t const width, uint32_t const height,
                                         size_t& rowBytes, uint32_t& rowCount) -> void
    {
        bool bc = false;
        uint32_t bpe = 0;

        switch (format)
        {
            case rhi::TextureFormat::BC1:
            case rhi::TextureFormat::BC4: {
                bc = true;
                bpe = 8;
                break;
            }
            case rhi::TextureFormat::BC3:
            case rhi::TextureFormat::BC5: {
                bc = true;
                bpe = 16;
                break;
            }
        }

        if (bc)
        {
            uint64_t const blockWide = std::max<uint64_t>(1u, (static_cast<uint64_t>(width) + 3u) / 4u);
            uint64_t const blockHigh = std::max<uint64_t>(1u, (static_cast<uint64_t>(height) + 3u) / 4u);
            rowBytes = blockWide * bpe;
            rowCount = static_cast<uint32_t>(blockHigh);
        }
        else
        {
            size_t const bpp = 32;
            rowBytes = (width * bpp + 7) / 8;
            rowCount = height;
        }
    }

    DX12CopyContext::DX12CopyContext(ID3D12Device4* device, D3D12MA::Allocator* memoryAllocator,
                                     DeviceQueueData& deviceQueue, HANDLE fenceEvent,
                                     RHICreateInfo const& rhiCreateInfo)
        : device(device), deviceQueue(&deviceQueue), fenceEvent(fenceEvent)
    {
        throwIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, __uuidof(ID3D12CommandAllocator),
                                                     commandAllocator.put_void()));

        throwIfFailed(device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE,
                                                 __uuidof(ID3D12GraphicsCommandList4), commandList.put_void()));

        {
            BufferCreateInfo const bufferCreateInfo{.size = rhiCreateInfo.stagingBufferSize,
                                                    .flags = (BufferUsageFlags)(BufferUsage::MapWrite)};
            writeStagingBuffer = {.buffer =
                                      core::make_ref<DX12Buffer>(device, memoryAllocator, nullptr, bufferCreateInfo)};
        }

        {
            BufferCreateInfo const bufferCreateInfo{.size = rhiCreateInfo.stagingBufferSize,
                                                    .flags = (BufferUsageFlags)(BufferUsage::MapRead)};
            readStagingBuffer = {.buffer =
                                     core::make_ref<DX12Buffer>(device, memoryAllocator, nullptr, bufferCreateInfo)};
        }
    }

    auto DX12CopyContext::tryResetCommandList() -> void
    {
        if (isCommandListOpened)
        {
            return;
        }

        throwIfFailed(commandAllocator->Reset());
        throwIfFailed(commandList->Reset(commandAllocator.get(), nullptr));

        isCommandListOpened = true;
    }

    auto DX12CopyContext::updateBuffer(core::ref_ptr<Buffer> dest, uint64_t const offset,
                                       std::span<uint8_t const> const dataBytes) -> Future<Buffer>
    {
        // Check for overflow staging buffer
        if (writeStagingBuffer.buffer->getSize() - writeStagingBuffer.offset < dataBytes.size())
        {
            auto executeResult = this->execute();
            // Wait until command buffer ends work
            executeResult.wait();
        }

        uint32_t const resourceAlignmentMask = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1;

        uint64_t const startWriteOffset = writeStagingBuffer.offset;

        uint8_t* mappedBytes = writeStagingBuffer.buffer->mapMemory();
        {
            std::basic_ospanstream<uint8_t> oss(
                std::span<uint8_t>(mappedBytes + startWriteOffset, writeStagingBuffer.buffer->getSize()));
            oss.write(dataBytes.data(), dataBytes.size());

            uint64_t const offset = oss.tellp();
            writeStagingBuffer.offset = (offset + resourceAlignmentMask) & ~resourceAlignmentMask;
        }
        writeStagingBuffer.buffer->unmapMemory();

        auto dxDestBuffer = dynamic_cast<DX12Buffer*>(dest.get());

        commandList->CopyBufferRegion(dxDestBuffer->getResource(), offset, writeStagingBuffer.buffer->getResource(),
                                      startWriteOffset, dataBytes.size());

        auto futureImpl = std::make_unique<DX12FutureImpl>(deviceQueue->queue.get(), deviceQueue->fence.get(),
                                                           fenceEvent, deviceQueue->fenceValue);
        return Future<Buffer>(dest, std::move(futureImpl));
    }

    /*
    auto DX12CopyContext::writeBuffer(core::ref_ptr<Buffer> dest, std::span<uint8_t const> const dataBytes)
        -> Future<Buffer>
    {
        // Check for overflow buffer
        if (writeStagingBuffer.buffer->getSize() - writeStagingBuffer.offset < dataBytes.size())
        {
            this->execute();
            this->reset();
        }

        uint32_t const resourceAlignmentMask = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1;

        uint64_t offset = writeStagingBuffer.offset;
        uint8_t* mappedBytes = writeStagingBuffer.buffer->mapMemory();
        {
            std::basic_ospanstream<uint8_t> stream(
                std::span<uint8_t>(mappedBytes + writeStagingBuffer.offset, writeStagingBuffer.buffer->getSize()),
                std::ios::binary);
            stream.write(dataBytes.data(), dataBytes.size());

            writeStagingBuffer.offset =
                (static_cast<uint64_t>(stream.tellp()) + resourceAlignmentMask) & ~resourceAlignmentMask;
        }
        writeStagingBuffer.buffer->unmapMemory();

        commandList->CopyBufferRegion(dynamic_cast<DX12Buffer*>(dest.get())->getResource(), 0,
                                      dynamic_cast<DX12Buffer*>(writeStagingBuffer.buffer.get())->getResource(), offset,
                                      dataBytes.size());

        auto futureImpl = std::make_unique<DX12FutureImpl>(queue, fence, fenceEvent, *fenceValue);
        return Future<Buffer>(dest, std::move(futureImpl));
    }

    auto DX12CopyContext::writeTexture(core::ref_ptr<Texture> dest, uint32_t const mipLevel,
                                       std::span<uint8_t const> const dataBytes) -> Future<Texture>
    {
        size_t rowBytes = 0;
        uint32_t rowCount = 0;
        size_t totalBytes = 0;

        uint32_t const rowPitchAlignmentMask = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1;
        uint32_t const resourceAlignmentMask = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1;
        uint32_t const stride = 4;

        auto resourceDesc = dynamic_cast<DX12Texture*>(dest.get())->getResource()->GetDesc();

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
        device->GetCopyableFootprints(&resourceDesc, mipLevel, 1, 0, &footprint, nullptr, nullptr, &totalBytes);

        // Check for overflow buffer
        if (writeStagingBuffer.buffer->getSize() - writeStagingBuffer.offset < totalBytes)
        {
            this->execute();
            this->reset();
        }

        uint64_t offset = writeStagingBuffer.offset;
        uint8_t* mappedBytes = writeStagingBuffer.buffer->mapMemory();
        {
            std::basic_ospanstream<uint8_t> stream(
                std::span<uint8_t>(mappedBytes + writeStagingBuffer.offset, writeStagingBuffer.buffer->getSize()),
                std::ios::binary);

            this->getSurfaceData(dest->getFormat(), footprint.Footprint.Width, footprint.Footprint.Height, rowBytes,
                                 rowCount);

            footprint.Footprint.RowPitch =
                (footprint.Footprint.Width * stride * rowPitchAlignmentMask) & ~rowPitchAlignmentMask;
            footprint.Offset = offset;

            for (uint32_t const i : std::views::iota(0u, rowCount))
            {
                stream.write(dataBytes.data() + rowBytes * i, rowBytes);

                // Align by Row Pitch Alignment (256)
                stream.seekp(footprint.Footprint.RowPitch - rowBytes, std::ios::cur);
            }

            size_t const totalSize = stream.tellp();

            // Align by Resource Alignment (512)
            stream.seekp(((totalSize + resourceAlignmentMask) & ~resourceAlignmentMask) - totalSize, std::ios::cur);

            writeStagingBuffer.offset = stream.tellp();
        }
        writeStagingBuffer.buffer->unmapMemory();

        D3D12_TEXTURE_COPY_LOCATION sourceCopyLocation{
            .pResource = dynamic_cast<DX12Buffer*>(writeStagingBuffer.buffer.get())->getResource(),
            .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT};
        D3D12_TEXTURE_COPY_LOCATION destCopyLocation{.pResource = dynamic_cast<DX12Texture*>(dest.get())->getResource(),
                                                     .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
                                                     .SubresourceIndex = mipLevel};
        commandList->CopyTextureRegion(&destCopyLocation, 0, 0, 0, &sourceCopyLocation, nullptr);

        auto futureImpl = std::make_unique<DX12FutureImpl>(queue, fence, fenceEvent, *fenceValue);
        return Future<Texture>(dest, std::move(futureImpl));
    }

    auto DX12CopyContext::readBuffer(core::ref_ptr<Buffer> dest, uint8_t* dataBytes) -> size_t
    {
        commandList->CopyBufferRegion(dynamic_cast<DX12Buffer*>(readStagingBuffer.buffer.get())->getResource(), 0,
                                      dynamic_cast<DX12Buffer*>(dest.get())->getResource(), 0, dest->getSize());

        // Execute immediately and wait
        Future<Query> result = this->execute();
        result.wait();

        this->reset();

        size_t readBytes = 0;
        uint8_t* mappedBytes = readStagingBuffer.buffer->mapMemory();
        {
            std::basic_ispanstream<uint8_t> stream(std::span<uint8_t>(mappedBytes, readStagingBuffer.buffer->getSize()),
                                                   std::ios::binary);
            stream.read(dataBytes, dest->getSize());
            readBytes = stream.gcount();
        }
        readStagingBuffer.buffer->unmapMemory();
        return readBytes;
    }

    auto DX12CopyContext::readTexture(core::ref_ptr<Texture> dest, uint32_t const mipLevel, uint8_t* dataBytes)
        -> size_t
    {
        size_t rowBytes = 0;
        uint32_t rowCount = 0;
        size_t totalBytes = 0;

        uint32_t const resourceAlignmentMask = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1;

        auto resourceDesc = dynamic_cast<DX12Texture*>(dest.get())->getResource()->GetDesc();

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
        device->GetCopyableFootprints(&resourceDesc, mipLevel, 1, 0, &footprint, nullptr, nullptr, &totalBytes);

        D3D12_TEXTURE_COPY_LOCATION sourceCopyLocation{.pResource =
                                                           dynamic_cast<DX12Texture*>(dest.get())->getResource(),
                                                       .Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX,
                                                       .SubresourceIndex = mipLevel};
        D3D12_TEXTURE_COPY_LOCATION destCopyLocation{
            .pResource = dynamic_cast<DX12Buffer*>(readStagingBuffer.buffer.get())->getResource(),
            .Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT,
            .PlacedFootprint = 0};
        commandList->CopyTextureRegion(&destCopyLocation, 0, 0, 0, &sourceCopyLocation, nullptr);

        // Execute immediately and wait
        Future<Query> result = this->execute();
        result.wait();

        this->reset();

        size_t readBytes = 0;
        uint8_t* mappedBytes = readStagingBuffer.buffer->mapMemory();
        {
            std::basic_ispanstream<uint8_t> stream(
                std::span<uint8_t>(mappedBytes + readStagingBuffer.offset, readStagingBuffer.buffer->getSize()),
                std::ios::binary);

            this->getSurfaceData(dest->getFormat(), footprint.Footprint.Width, footprint.Footprint.Height, rowBytes,
                                 rowCount);

            for (uint32_t const i : std::views::iota(0u, rowCount))
            {
                stream.read(dataBytes + rowBytes * i, rowBytes);
                stream.seekg(footprint.Footprint.RowPitch - rowBytes, std::ios::cur);

                readBytes += rowBytes;
            }
        }
        readStagingBuffer.buffer->unmapMemory();
        return readBytes;
    }*/

    auto DX12CopyContext::execute() -> Future<void>
    {
        writeStagingBuffer.offset = 0;

        throwIfFailed(commandList->Close());
        isCommandListOpened = false;

        std::array<ID3D12CommandList*, 1> const commandLists{commandList.get()};
        deviceQueue->queue->ExecuteCommandLists(static_cast<uint32_t>(commandLists.size()), commandLists.data());

        deviceQueue->fenceValue++;
        throwIfFailed(deviceQueue->queue->Signal(deviceQueue->fence.get(), deviceQueue->fenceValue));

        auto futureImpl = std::make_unique<DX12FutureImpl>(deviceQueue->queue.get(), deviceQueue->fence.get(),
                                                           fenceEvent, deviceQueue->fenceValue);
        return Future<void>(std::move(futureImpl));
    }

    auto DX12CopyContext::barrier(core::ref_ptr<Buffer> dest, ResourceState const before, ResourceState const after)
        -> void
    {
        DX12DeviceContext_barrier(commandList.get(), dest, before, after);
    }

    auto DX12CopyContext::barrier(core::ref_ptr<Texture> dest, ResourceState const before, ResourceState const after)
        -> void
    {
        DX12DeviceContext_barrier(commandList.get(), dest, before, after);
    }

    DX12Swapchain::DX12Swapchain(IDXGIFactory4* factory, ID3D12Device4* device,
                                 DescriptorAllocator* descriptorAllocator, DeviceQueueData& deviceQueue,
                                 HANDLE fenceEvent, SwapchainCreateInfo const& createInfo)
        : device(device), descriptorAllocator(descriptorAllocator), deviceQueue(&deviceQueue), fenceEvent(fenceEvent)
    {
        RECT rect{};
        ::GetClientRect(reinterpret_cast<HWND>(createInfo.window), &rect);

        {
            DXGI_SWAP_CHAIN_DESC1 const swapchainDesc{.Width = static_cast<uint32_t>(rect.right),
                                                      .Height = static_cast<uint32_t>(rect.bottom),
                                                      .Format = DXGI_FORMAT_R8G8B8A8_UNORM,
                                                      .SampleDesc = {.Count = 1},
                                                      .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
                                                      .BufferCount = 2,
                                                      .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
                                                      .AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED};

            winrt::com_ptr<IDXGISwapChain1> swapchain;
            throwIfFailed(factory->CreateSwapChainForHwnd(this->deviceQueue->queue.get(),
                                                          reinterpret_cast<HWND>(createInfo.window), &swapchainDesc,
                                                          nullptr, nullptr, swapchain.put()));
            swapchain.as(this->swapchain);
        }

        this->createSwapchainBuffers(rect.right, rect.bottom);
    }

    auto DX12Swapchain::requestBackBuffer() -> core::weak_ptr<Texture>
    {
        if (deviceQueue->fence->GetCompletedValue() < deviceQueue->fenceValue)
        {
            throwIfFailed(deviceQueue->fence->SetEventOnCompletion(deviceQueue->fenceValue, fenceEvent));
            ::WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
        }
        return backBuffers[swapchain->GetCurrentBackBufferIndex()];
    }

    auto DX12Swapchain::presentBackBuffer() -> void
    {
        throwIfFailed(swapchain->Present(0, 0));

        deviceQueue->fenceValue++;
        throwIfFailed(deviceQueue->queue->Signal(deviceQueue->fence.get(), deviceQueue->fenceValue));
    }

    auto DX12Swapchain::resizeBackBuffers(uint32_t const width, uint32_t const height) -> void
    {
        throwIfFailed(deviceQueue->fence->SetEventOnCompletion(deviceQueue->fenceValue, fenceEvent));
        ::WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);

        backBuffers.clear();

        throwIfFailed(swapchain->ResizeBuffers(2, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));
        this->createSwapchainBuffers(width, height);
    }

    auto DX12Swapchain::createSwapchainBuffers(uint32_t const width, uint32_t const height) -> void
    {
        for (uint32_t const i : std::views::iota(0u, 2u))
        {
            core::ref_ptr<Texture> texture;
            {
                winrt::com_ptr<ID3D12Resource> resource;
                throwIfFailed(swapchain->GetBuffer(i, __uuidof(ID3D12Resource), resource.put_void()));
                texture = core::make_ref<DX12Texture>(device, descriptorAllocator, resource);
            }
            backBuffers.emplace_back(texture);
        }
    }

    DX12RHI::DX12RHI(RHICreateInfo const& createInfo)
    {
#ifndef NDEBUG
        throwIfFailed(::D3D12GetDebugInterface(__uuidof(ID3D12Debug1), debug.put_void()));
        debug->EnableDebugLayer();
        debug->SetEnableGPUBasedValidation(true);
        debug->SetEnableSynchronizedCommandQueueValidation(true);

        uint32_t const factoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#else
        uint32_t const factoryFlags = 0;
#endif

        // Use Release build for deploy to users (Debug builds requires D3D12 Debug Tools, or you got exception)
        throwIfFailed(::CreateDXGIFactory2(factoryFlags, __uuidof(IDXGIFactory4), factory.put_void()));

        // Pick first non-software adapter
        {
            winrt::com_ptr<IDXGIAdapter1> adapter;

            for (uint32_t i = 0; factory->EnumAdapters1(i, adapter.put()) != DXGI_ERROR_NOT_FOUND; ++i)
            {
                DXGI_ADAPTER_DESC1 adapterDesc{};
                throwIfFailed(adapter->GetDesc1(&adapterDesc));

                if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
                {
                    continue;
                }
                else
                {
                    this->adapter = adapter;
                    break;
                }
            }
        }

        throwIfFailed(
            ::D3D12CreateDevice(adapter.get(), D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device4), device.put_void()));

        fenceEvent = UniqueHandle(::CreateEvent(nullptr, FALSE, FALSE, nullptr));
        if (!fenceEvent)
        {
            throwIfFailed(::HRESULT_FROM_WIN32(GetLastError()));
        }

        this->createDeviceQueue(D3D12_COMMAND_LIST_TYPE_DIRECT, graphicsQueue);
        this->createDeviceQueue(D3D12_COMMAND_LIST_TYPE_COPY, copyQueue);
        this->createDeviceQueue(D3D12_COMMAND_LIST_TYPE_COMPUTE, computeQueue);

        descriptorAllocator = core::make_ref<DescriptorAllocator>(device.get());

        D3D12MA::ALLOCATOR_DESC const memoryAllocatorDesc{.pDevice = device.get(), .pAdapter = adapter.get()};
        throwIfFailed(D3D12MA::CreateAllocator(&memoryAllocatorDesc, memoryAllocator.put()));

        pipelineCache = core::make_ref<PipelineCache>(device.get(), createInfo);

        graphicsContext = core::make_ref<DX12GraphicsContext>(
            device.get(), pipelineCache.get(), descriptorAllocator.get(), graphicsQueue, fenceEvent.get());
        copyContext = core::make_ref<DX12CopyContext>(device.get(), memoryAllocator.get(), copyQueue, fenceEvent.get(),
                                                      createInfo);
    }

    auto DX12RHI::createShader(ShaderCreateInfo const& createInfo) -> core::ref_ptr<Shader>
    {
        return core::make_ref<DX12Shader>(device.get(), createInfo);
    }

    auto DX12RHI::createTexture(TextureCreateInfo const& createInfo) -> core::ref_ptr<Texture>
    {
        return core::make_ref<DX12Texture>(device.get(), memoryAllocator.get(), descriptorAllocator.get(), createInfo);
    }

    auto DX12RHI::createBuffer(BufferCreateInfo const& createInfo) -> core::ref_ptr<Buffer>
    {
        return core::make_ref<DX12Buffer>(device.get(), memoryAllocator.get(), descriptorAllocator.get(), createInfo);
    }

    auto DX12RHI::createSampler(SamplerCreateInfo const& createInfo) -> core::ref_ptr<Sampler>
    {
        return core::make_ref<DX12Sampler>(device.get(), descriptorAllocator.get(), createInfo);
    }

    auto DX12RHI::tryGetSwapchain(SwapchainCreateInfo const& createInfo) -> core::ref_ptr<Swapchain>
    {
        if (!swapchain)
        {
            swapchain = core::make_ref<DX12Swapchain>(factory.get(), device.get(), descriptorAllocator.get(),
                                                      graphicsQueue, fenceEvent.get(), createInfo);
        }

        return swapchain;
    }

    auto DX12RHI::getGraphicsContext() -> core::ref_ptr<GraphicsContext>
    {
        return graphicsContext;
    }

    auto DX12RHI::getCopyContext() -> core::ref_ptr<CopyContext>
    {
        return copyContext;
    }

    auto DX12RHI::getName() const -> std::string_view
    {
        return rhiName;
    }

    auto DX12RHI::createDeviceQueue(D3D12_COMMAND_LIST_TYPE const commandListType, DeviceQueueData& deviceQueue) -> void
    {
        D3D12_COMMAND_QUEUE_DESC const commandQueueDesc{.Type = commandListType,
                                                        .Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL};
        throwIfFailed(
            device->CreateCommandQueue(&commandQueueDesc, __uuidof(ID3D12CommandQueue), deviceQueue.queue.put_void()));
        throwIfFailed(
            device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), deviceQueue.fence.put_void()));
        deviceQueue.fenceValue = deviceQueue.fence->GetCompletedValue();
    }
} // namespace ionengine::rhi