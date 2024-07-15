// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#include "rhi.hpp"
#include "platform/window.hpp"
#include "precompiled.h"

namespace ionengine::rhi
{
    auto HRESULT_to_string(HRESULT const hr) -> std::string
    {
        switch (hr)
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

    auto throwIfFailed(HRESULT hr) -> void
    {
        if (FAILED(hr))
        {
            throw core::Exception(HRESULT_to_string(hr));
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
            case ResourceState::DepthRead:
                return D3D12_RESOURCE_STATE_DEPTH_READ;
            case ResourceState::DepthWrite:
                return D3D12_RESOURCE_STATE_DEPTH_WRITE;
            case ResourceState::RenderTarget:
                return D3D12_RESOURCE_STATE_RENDER_TARGET;
            case ResourceState::UnorderedAccess:
                return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            case ResourceState::PixelShaderRead:
                return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
            case ResourceState::NonPixelShaderRead:
                return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
            case ResourceState::AllShaderRead:
                return D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE;
            case ResourceState::CopySrc:
                return D3D12_RESOURCE_STATE_COPY_SOURCE;
            case ResourceState::CopyDst:
                return D3D12_RESOURCE_STATE_COPY_DEST;
            default:
                throw std::invalid_argument("Invalid argument for conversion");
        }
    }

    auto VertexFormat_to_DXGI_FORMAT(VertexFormat const format) -> DXGI_FORMAT
    {
        switch (format)
        {
            case VertexFormat::Float4:
            case VertexFormat::Float4x4:
                return DXGI_FORMAT_R32G32B32A32_FLOAT;
            case VertexFormat::Float3:
            case VertexFormat::Float3x3:
                return DXGI_FORMAT_R32G32B32_FLOAT;
            case VertexFormat::Float2:
            case VertexFormat::Float2x2:
                return DXGI_FORMAT_R32G32_FLOAT;
            case VertexFormat::Float:
                return DXGI_FORMAT_R32_FLOAT;
            case VertexFormat::Uint:
                return DXGI_FORMAT_R32_UINT;
            default:
                return DXGI_FORMAT_UNKNOWN;
        }
    }

    DescriptorAllocator::DescriptorAllocator(ID3D12Device1* device) : device(device)
    {
        this->createChunk(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        this->createChunk(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        this->createChunk(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        this->createChunk(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    }

    auto DescriptorAllocator::allocate(D3D12_DESCRIPTOR_HEAP_TYPE const heapType,
                                       uint32_t const allocSize) -> DescriptorAllocation
    {
        assert((chunks.find(heapType) != chunks.end()) && "Required heap not found when allocating descriptor");

        auto allocation = DescriptorAllocation{};
        auto& chunk = chunks[heapType];

        if (allocSize > chunk.size - chunk.offset)
        {
            throw core::Exception("Required heap does not contain free descriptors");
        }

        uint32_t allocStart = chunk.offset;
        uint32_t requiredSize = 0;
        bool success = false;

        for (uint32_t const i : std::views::iota(chunk.offset, chunk.size))
        {
            if (requiredSize == allocSize)
            {
                success = true;
                break;
            }

            if (chunk.free[i] == 1)
            {
                requiredSize++;
            }
            else
            {
                allocStart = i + 1;
                requiredSize = 0;
            }
        }

        if (success)
        {
            std::fill(chunk.free.begin() + allocStart, chunk.free.begin() + allocStart + requiredSize, 0);
            chunk.offset += requiredSize;

            allocation = {.heap = chunk.heap.get(),
                          .heapType = heapType,
                          .incrementSize = device->GetDescriptorHandleIncrementSize(heapType),
                          .offset = allocStart,
                          .size = allocSize};
        }
        else
        {
            throw core::Exception("Required heap does not contain free descriptors");
        }
        return allocation;
    }

    auto DescriptorAllocator::deallocate(DescriptorAllocation const& allocation) -> void
    {
        assert((chunks.find(allocation.heapType) != chunks.end()) &&
               "Required heap not found when deallocating descriptor");

        auto& chunk = chunks[allocation.heapType];
        std::fill(chunk.free.begin() + allocation.offset, chunk.free.begin() + allocation.offset + allocation.size, 1);
        chunk.offset = allocation.offset;
    }

    auto DescriptorAllocator::createChunk(D3D12_DESCRIPTOR_HEAP_TYPE const heapType) -> void
    {
        uint32_t allocSize = 0;
        D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        switch (heapType)
        {
            case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV: {
                allocSize = std::to_underlying(DescriptorAllocatorLimits::CBV_SRV_UAV);
                flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                break;
            }
            case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: {
                allocSize = std::to_underlying(DescriptorAllocatorLimits::RTV);
                break;
            }
            case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: {
                allocSize = std::to_underlying(DescriptorAllocatorLimits::DSV);
                break;
            }
            case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER: {
                allocSize = std::to_underlying(DescriptorAllocatorLimits::Sampler);
                flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                break;
            }
        }

        auto d3d12DescriptorHeapDesc = D3D12_DESCRIPTOR_HEAP_DESC{};
        d3d12DescriptorHeapDesc.NumDescriptors = allocSize;
        d3d12DescriptorHeapDesc.Type = heapType;
        d3d12DescriptorHeapDesc.Flags = flags;

        winrt::com_ptr<ID3D12DescriptorHeap> descriptor_heap;
        throwIfFailed(device->CreateDescriptorHeap(&d3d12DescriptorHeapDesc, __uuidof(ID3D12DescriptorHeap),
                                                   descriptor_heap.put_void()));

        auto chunk = Chunk{.heap = descriptor_heap, .offset = 0, .size = allocSize};
        chunk.free.resize(allocSize, 1);
        chunks.emplace(heapType, chunk);
    }

    auto DescriptorAllocator::getHeap(D3D12_DESCRIPTOR_HEAP_TYPE const heapType) -> ID3D12DescriptorHeap*
    {
        return chunks[heapType].heap.get();
    }

    DX12Buffer::DX12Buffer(ID3D12Device1* device, D3D12MA::Allocator* memoryAllocator,
                           DescriptorAllocator* descriptorAllocator, BufferCreateInfo const& createInfo)
        : memoryAllocator(memoryAllocator), descriptorAllocator(descriptorAllocator), size(createInfo.size),
          flags(createInfo.flags)
    {
        auto d3d12ResourceDesc = D3D12_RESOURCE_DESC{};
        d3d12ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        if (flags & BufferUsage::ConstantBuffer)
        {
            d3d12ResourceDesc.Width =
                static_cast<uint64_t>((static_cast<uint32_t>(size) + (DX12Buffer::ConstantBufferSizeAlignment - 1)) &
                                      ~(DX12Buffer::ConstantBufferSizeAlignment - 1));
        }
        else
        {
            d3d12ResourceDesc.Width = size;
        }
        d3d12ResourceDesc.Height = 1;
        d3d12ResourceDesc.MipLevels = 1;
        d3d12ResourceDesc.DepthOrArraySize = 1;
        d3d12ResourceDesc.SampleDesc.Count = 1;
        d3d12ResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        d3d12ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

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

        auto d3d12maAllocationDesc = D3D12MA::ALLOCATION_DESC{};
        d3d12maAllocationDesc.HeapType = heapType;

        throwIfFailed(memoryAllocator->CreateResource(&d3d12maAllocationDesc, &d3d12ResourceDesc, initialState, nullptr,
                                                      memoryAllocation.put(), __uuidof(ID3D12Resource),
                                                      resource.put_void()));

        if (flags & BufferUsage::ConstantBuffer)
        {
            assert(descriptorAllocator && "To create a buffer with views, you need to pass the allocator descriptor");

            auto allocation = descriptorAllocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a buffer");
            }
            descriptorAllocations.emplace(BufferUsage::ConstantBuffer, allocation);

            auto d3d12ConstantBufferViewDesc = D3D12_CONSTANT_BUFFER_VIEW_DESC{};
            d3d12ConstantBufferViewDesc.BufferLocation = resource->GetGPUVirtualAddress();
            d3d12ConstantBufferViewDesc.SizeInBytes = static_cast<uint32_t>(d3d12ResourceDesc.Width);

            device->CreateConstantBufferView(&d3d12ConstantBufferViewDesc, allocation.cpuHandle());
        }
        if (flags & BufferUsage::ShaderResource)
        {
            assert(descriptorAllocator && "To create a buffer with views, you need to pass the allocator descriptor");

            auto allocation = descriptorAllocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a buffer");
            }
            descriptorAllocations.emplace(BufferUsage::ShaderResource, allocation);

            auto d3d12ShaderResourceViewDesc = D3D12_SHADER_RESOURCE_VIEW_DESC{};
            d3d12ShaderResourceViewDesc.Buffer.FirstElement = 0;
            d3d12ShaderResourceViewDesc.Buffer.NumElements = size / createInfo.elementStride;
            d3d12ShaderResourceViewDesc.Buffer.StructureByteStride = createInfo.elementStride;

            device->CreateShaderResourceView(resource.get(), &d3d12ShaderResourceViewDesc, allocation.cpuHandle());
        }
        if (flags & BufferUsage::UnorderedAccess)
        {
            assert(descriptorAllocator && "To create a buffer with views, you need to pass the allocator descriptor");

            auto allocation = descriptorAllocator->allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a buffer");
            }
            descriptorAllocations.emplace(BufferUsage::UnorderedAccess, allocation);

            auto d3d12UnorderedAccessViewDesc = D3D12_UNORDERED_ACCESS_VIEW_DESC{};
            d3d12UnorderedAccessViewDesc.Buffer.FirstElement = 0;
            d3d12UnorderedAccessViewDesc.Buffer.NumElements = size / createInfo.elementStride;
            d3d12UnorderedAccessViewDesc.Buffer.StructureByteStride = createInfo.elementStride;

            device->CreateUnorderedAccessView(resource.get(), nullptr, &d3d12UnorderedAccessViewDesc,
                                              allocation.cpuHandle());
        }
    }

    DX12Buffer::~DX12Buffer()
    {
        if (descriptorAllocator)
        {
            for (auto const& [usage, allocation] : descriptorAllocations)
            {
                descriptorAllocator->deallocate(allocation);
            }
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
            auto range = D3D12_RANGE{.Begin = 0, .End = size};
            throwIfFailed(resource->Map(0, &range, reinterpret_cast<void**>(&mappedBytes)));
        }
        return mappedBytes;
    }

    auto DX12Buffer::unmapMemory() -> void
    {
        if (flags & BufferUsage::MapWrite)
        {
            auto range = D3D12_RANGE{.Begin = 0, .End = size};
            resource->Unmap(0, &range);
        }
        else if (flags & BufferUsage::MapRead)
        {
            resource->Unmap(0, nullptr);
        }
    }

    auto DX12Buffer::getSize() -> size_t
    {
        return size;
    }

    auto DX12Buffer::getFlags() -> BufferUsageFlags
    {
        return flags;
    }

    auto DX12Buffer::getResource() -> ID3D12Resource*
    {
        return resource.get();
    }

    auto DX12Buffer::getDescriptor(BufferUsage const usage) const -> DescriptorAllocation const&
    {
        return descriptorAllocations.at(usage);
    }

    auto DX12Buffer::getDescriptorOffset(BufferUsage const usage) const -> uint32_t
    {
        return descriptorAllocations.at(usage).offset;
    }

    DX12Texture::DX12Texture(ID3D12Device1* device, D3D12MA::Allocator* memoryAllocator,
                             DescriptorAllocator& descriptorAllocator, TextureCreateInfo const& createInfo)
        : memoryAllocator(memoryAllocator), descriptorAllocator(&descriptorAllocator), width(createInfo.width),
          height(createInfo.height), depth(createInfo.depth), mipLevels(createInfo.mipLevels),
          format(createInfo.format), dimension(createInfo.dimension), flags(createInfo.flags)
    {
        auto d3d12ResourceDesc = D3D12_RESOURCE_DESC{};
        switch (dimension)
        {
            case TextureDimension::_1D: {
                d3d12ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE1D;
                break;
            }
            case TextureDimension::Cube:
            case TextureDimension::_2DArray:
            case TextureDimension::CubeArray:
            case TextureDimension::_2D: {
                d3d12ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
                break;
            }
            case TextureDimension::_3D: {
                d3d12ResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE3D;
                break;
            }
        }
        d3d12ResourceDesc.Width = width;
        d3d12ResourceDesc.Height = height;
        d3d12ResourceDesc.MipLevels = mipLevels;
        d3d12ResourceDesc.DepthOrArraySize = depth;
        d3d12ResourceDesc.SampleDesc.Count = 1;
        d3d12ResourceDesc.Format = TextureFormat_to_DXGI_FORMAT(format);
        d3d12ResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

        D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;
        D3D12_HEAP_TYPE heapType = D3D12_HEAP_TYPE_DEFAULT;

        if (flags & TextureUsage::DepthStencil)
        {
            d3d12ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
            initialState = D3D12_RESOURCE_STATE_DEPTH_READ;
        }
        if (flags & TextureUsage::RenderTarget)
        {
            d3d12ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
        }
        if (flags & TextureUsage::UnorderedAccess)
        {
            d3d12ResourceDesc.Flags |= D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;
        }

        auto d3d12maAllocationDesc = D3D12MA::ALLOCATION_DESC{};
        d3d12maAllocationDesc.HeapType = heapType;

        throwIfFailed(memoryAllocator->CreateResource(&d3d12maAllocationDesc, &d3d12ResourceDesc, initialState, nullptr,
                                                      memoryAllocation.put(), __uuidof(ID3D12Resource),
                                                      resource.put_void()));

        if (flags & TextureUsage::RenderTarget)
        {
            auto allocation = descriptorAllocator.allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a texture");
            }
            descriptorAllocations.emplace(TextureUsage::RenderTarget, allocation);

            auto d3d12RenderTargetViewDesc = D3D12_RENDER_TARGET_VIEW_DESC{};
            d3d12RenderTargetViewDesc.Format = d3d12ResourceDesc.Format;
            d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

            device->CreateRenderTargetView(resource.get(), &d3d12RenderTargetViewDesc, allocation.cpuHandle());
        }

        if (flags & TextureUsage::DepthStencil)
        {
            auto allocation = descriptorAllocator.allocate(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a texture");
            }
            descriptorAllocations.emplace(TextureUsage::DepthStencil, allocation);

            auto d3d12DepthStencilView = D3D12_DEPTH_STENCIL_VIEW_DESC{};
            d3d12DepthStencilView.Format = DXGI_FORMAT_D32_FLOAT;
            switch (dimension)
            {
                case TextureDimension::_2D: {
                    d3d12DepthStencilView.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
                    break;
                }
                default: {
                    assert(false && "Creation of depth stencil only possible in 2D dimension");
                    break;
                }
            }

            device->CreateDepthStencilView(resource.get(), &d3d12DepthStencilView, allocation.cpuHandle());
        }

        if (flags & TextureUsage::ShaderResource)
        {
            auto allocation = descriptorAllocator.allocate(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a texture");
            }
            descriptorAllocations.emplace(TextureUsage::ShaderResource, allocation);

            auto d3d12ShaderResourceView = D3D12_SHADER_RESOURCE_VIEW_DESC{};
            d3d12ShaderResourceView.Format = d3d12ResourceDesc.Format;
            d3d12ShaderResourceView.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            switch (dimension)
            {
                case TextureDimension::_1D: {
                    d3d12ShaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE1D;
                    d3d12ShaderResourceView.Texture1D.MipLevels = mipLevels;
                    break;
                }
                case TextureDimension::_2D: {
                    d3d12ShaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                    d3d12ShaderResourceView.Texture2D.MipLevels = mipLevels;
                    break;
                }
                case TextureDimension::_3D: {
                    d3d12ShaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE3D;
                    d3d12ShaderResourceView.Texture3D.MipLevels = mipLevels;
                    break;
                }
                case TextureDimension::Cube: {
                    d3d12ShaderResourceView.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                    d3d12ShaderResourceView.Texture3D.MipLevels = mipLevels;
                    break;
                }
            }

            device->CreateShaderResourceView(resource.get(), &d3d12ShaderResourceView, allocation.cpuHandle());
        }
    }

    DX12Texture::DX12Texture(ID3D12Device1* device, winrt::com_ptr<ID3D12Resource> resource,
                             DescriptorAllocator& descriptorAllocator)
        : memoryAllocator(nullptr), descriptorAllocator(&descriptorAllocator), resource(resource),
          flags((TextureUsageFlags)TextureUsage::RenderTarget)
    {
        auto d3d12ResourceDesc = resource->GetDesc();
        width = static_cast<uint32_t>(d3d12ResourceDesc.Width);
        height = d3d12ResourceDesc.Height;
        depth = d3d12ResourceDesc.DepthOrArraySize;
        mipLevels = d3d12ResourceDesc.MipLevels;
        format = DXGI_FORMAT_to_TextureFormat(d3d12ResourceDesc.Format);

        if (flags & TextureUsage::RenderTarget)
        {
            auto allocation = descriptorAllocator.allocate(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 1);
            if (allocation.size == 0)
            {
                throw core::Exception("An error in descriptor allocation when creating a texture");
            }
            descriptorAllocations.emplace(TextureUsage::RenderTarget, allocation);

            auto d3d12RenderTargetViewDesc = D3D12_RENDER_TARGET_VIEW_DESC{};
            d3d12RenderTargetViewDesc.Format = d3d12ResourceDesc.Format;
            d3d12RenderTargetViewDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

            device->CreateRenderTargetView(resource.get(), &d3d12RenderTargetViewDesc, allocation.cpuHandle());
        }
    }

    DX12Texture::~DX12Texture()
    {
        for (auto const& [usage, allocation] : descriptorAllocations)
        {
            descriptorAllocator->deallocate(allocation);
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

    auto DX12Texture::getDescriptor(TextureUsage const usage) const -> DescriptorAllocation const&
    {
        return descriptorAllocations.at(usage);
    }

    auto DX12Texture::getResource() -> ID3D12Resource*
    {
        return resource.get();
    }

    auto DX12Texture::getDescriptorOffset(TextureUsage const usage) const -> uint32_t
    {
        return descriptorAllocations.at(usage).offset;
    }

    auto sizeof_VertexFormat(VertexFormat const format) -> uint32_t
    {
        switch (format)
        {
            case VertexFormat::Float4:
                return sizeof(float) * 4;
            case VertexFormat::Float4x4:
                return sizeof(float) * 16;
            case VertexFormat::Float3:
                return sizeof(float) * 3;
            case VertexFormat::Float3x3:
                return sizeof(float) * 9;
            case VertexFormat::Float2:
                return sizeof(float) * 2;
            case VertexFormat::Float2x2:
                return sizeof(float) * 4;
            case VertexFormat::Float:
                return sizeof(float);
            case VertexFormat::Uint:
                return sizeof(uint32_t);
            default:
                return 0;
        }
    }

    DX12Shader::DX12Shader(ID3D12Device4* device, ShaderCreateInfo const& createInfo)
    {
        
    }

    DX12Shader::DX12Shader(ID3D12Device4* device, std::span<VertexDeclarationInfo const> const vertexDeclarations,
                           std::span<uint8_t const> const vertexShader, std::span<uint8_t const> const pixelShader)
    {
        using shader_stage_t = std::pair<D3D12_SHADER_TYPE, std::span<uint8_t const>>;
        auto shaders = std::vector<shader_stage_t>{{D3D12_SHADER_TYPE_VERTEX, vertexShader},
                                                   {D3D12_SHADER_TYPE_PIXEL, pixelShader}};

        XXHash64 hasher(0);
        for (auto const& shader : shaders)
        {
            std::vector<uint8_t> buffer;
            {
                buffer.resize(shader.second.size());
                std::memcpy(buffer.data(), shader.second.data(), buffer.size());
            }

            hasher.add(buffer.data(), buffer.size());
            buffers.emplace_back(std::move(buffer));

            auto d3d12_shader_bytecode = D3D12_SHADER_BYTECODE{};
            d3d12_shader_bytecode.pShaderBytecode = buffers.back().data();
            d3d12_shader_bytecode.BytecodeLength = buffers.back().size();

            stages.emplace(shader.first, d3d12_shader_bytecode);
        }
        hash = hasher.hash();

        std::vector<size_t> declarationIndices;
        for (auto const& vertexDeclaration : vertexDeclarations)
        {
            declarationIndices.emplace_back(vertexDeclaration.index);
        }

        std::sort(declarationIndices.begin(), declarationIndices.end(), [&](auto const& lhs, auto const& rhs) {
            return vertexDeclarations[lhs].index < vertexDeclarations[rhs].index;
        });

        for (auto const& declarationIndex : declarationIndices)
        {
            D3D12_INPUT_ELEMENT_DESC d3d12_input_element{};
            uint32_t semanticIndex;
            if (std::isdigit(vertexDeclarations[declarationIndex].semantic.back()) == 0)
            {
                inputAssembler.semanticNames.emplace_back(vertexDeclarations[declarationIndex].semantic);
                semanticIndex = 0;
            }
            else
            {
                semanticIndex = static_cast<int32_t>(vertexDeclarations[declarationIndex].semantic.back()) - 48;
                std::string const semanticName = vertexDeclarations[declarationIndex].semantic.substr(
                    0, vertexDeclarations[declarationIndex].semantic.size() - 1);
                inputAssembler.semanticNames.emplace_back(semanticName);
            }

            d3d12_input_element.SemanticName = inputAssembler.semanticNames.back().c_str();
            d3d12_input_element.SemanticIndex = semanticIndex;
            d3d12_input_element.Format = vertexFormatToDXGI(vertexDeclarations[declarationIndex].format);
            d3d12_input_element.InputSlot = 0;
            d3d12_input_element.AlignedByteOffset = inputAssembler.inputSize;
            d3d12_input_element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
            d3d12_input_element.InstanceDataStepRate = 0;

            inputAssembler.inputElements.emplace_back(d3d12_input_element);
            inputAssembler.inputSize += vertexFormatSize(vertexDeclarations[declarationIndex].format);
        }
    }

    DX12Shader::DX12Shader(ID3D12Device4* device, std::span<uint8_t const> const computeShader)
    {
        XXHash64 hasher(0);

        std::vector<uint8_t> buffer;
        {
            buffer.resize(computeShader.size());
            std::memcpy(buffer.data(), computeShader.data(), buffer.size());
        }

        hasher.add(buffer.data(), buffer.size());
        buffers.emplace_back(std::move(buffer));

        auto d3d12_shader_bytecode = D3D12_SHADER_BYTECODE{};
        d3d12_shader_bytecode.pShaderBytecode = buffers.back().data();
        d3d12_shader_bytecode.BytecodeLength = buffers.back().size();

        stages.emplace(D3D12_SHADER_TYPE_COMPUTE, d3d12_shader_bytecode);

        hash = hasher.hash();
    }

    auto DX12Shader::getStages() const -> std::unordered_map<D3D12_SHADER_TYPE, D3D12_SHADER_BYTECODE> const&
    {
        return stages;
    }

    auto DX12Shader::getHash() const -> uint64_t
    {
        return hash;
    }

    auto DX12Shader::getInputAssembler() const -> InputAssemblerInfo const&
    {
        return inputAssembler;
    }

    Pipeline::Pipeline(ID3D12Device4* device, ID3D12RootSignature* rootSignature, DX12Shader& shader,
                       RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                       std::optional<DepthStencilStageInfo> const depthStencil,
                       std::span<DXGI_FORMAT const> const renderTargetFormats, DXGI_FORMAT const depthStencilFormat,
                       ID3DBlob* blob)
        : rootSignature(rootSignature)
    {
        auto d3d12PipelineDesc = D3D12_GRAPHICS_PIPELINE_STATE_DESC{};
        d3d12PipelineDesc.pRootSignature = rootSignature;

        for (auto const& [stage, stageData] : shader.getStages())
        {
            switch (stage)
            {
                case D3D12_SHADER_TYPE_VERTEX: {
                    d3d12PipelineDesc.VS = stageData;

                    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

                    if (!shader.getInputAssembler().inputElements.empty())
                    {
                        inputLayoutDesc.pInputElementDescs = shader.getInputAssembler().inputElements.data();
                        inputLayoutDesc.NumElements =
                            static_cast<uint32_t>(shader.getInputAssembler().inputElements.size());
                    }

                    d3d12PipelineDesc.InputLayout = inputLayoutDesc;
                    break;
                }
                case D3D12_SHADER_TYPE_PIXEL: {
                    d3d12PipelineDesc.PS = stageData;

                    auto d3d12RenderTargetBlend = D3D12_RENDER_TARGET_BLEND_DESC{};
                    d3d12RenderTargetBlend.BlendEnable = blendColor.blendEnable;
                    d3d12RenderTargetBlend.SrcBlend = Blend_to_D3D12_BLEND(blendColor.blendSrc);
                    d3d12RenderTargetBlend.DestBlend = Blend_to_D3D12_BLEND(blendColor.blendDst);
                    d3d12RenderTargetBlend.BlendOp = BlendOp_to_D3D12_BLEND_OP(blendColor.blendOp);
                    d3d12RenderTargetBlend.SrcBlendAlpha = Blend_to_D3D12_BLEND(blendColor.blendSrcAlpha);
                    d3d12RenderTargetBlend.DestBlendAlpha = Blend_to_D3D12_BLEND(blendColor.blendDstAlpha);
                    d3d12RenderTargetBlend.BlendOpAlpha = BlendOp_to_D3D12_BLEND_OP(blendColor.blendOpAlpha);
                    d3d12RenderTargetBlend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

                    auto d3d12Blend = D3D12_BLEND_DESC{};
                    for (uint32_t const i : std::views::iota(0u, renderTargetFormats.size()))
                    {
                        if (renderTargetFormats[i] == DXGI_FORMAT_UNKNOWN)
                        {
                            break;
                        }

                        d3d12PipelineDesc.RTVFormats[i] = renderTargetFormats[i];
                        d3d12Blend.RenderTarget[i] = d3d12RenderTargetBlend;
                    }
                    d3d12PipelineDesc.NumRenderTargets = static_cast<uint32_t>(renderTargetFormats.size());
                    d3d12PipelineDesc.BlendState = d3d12Blend;
                    d3d12PipelineDesc.DSVFormat = depthStencilFormat;
                    break;
                }
            }
        }

        auto d3d12Rasterizer = D3D12_RASTERIZER_DESC{};
        d3d12Rasterizer.FillMode = FillMode_to_D3D12_FILL_MODE(rasterizer.fillMode);
        d3d12Rasterizer.CullMode = CullMode_to_D3D12_CULL_MODE(rasterizer.cullMode);
        d3d12Rasterizer.FrontCounterClockwise = true;
        d3d12Rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
        d3d12Rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
        d3d12Rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        d3d12Rasterizer.DepthClipEnable = true;
        d3d12Rasterizer.MultisampleEnable = false;
        d3d12Rasterizer.AntialiasedLineEnable = false;
        d3d12Rasterizer.ForcedSampleCount = 0;
        d3d12Rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

        d3d12PipelineDesc.RasterizerState = d3d12Rasterizer;

        d3d12PipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        d3d12PipelineDesc.SampleMask = std::numeric_limits<uint32_t>::max();
        d3d12PipelineDesc.SampleDesc.Count = 1;

        auto d3d12DepthStencilFace = D3D12_DEPTH_STENCILOP_DESC{};
        d3d12DepthStencilFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
        d3d12DepthStencilFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
        d3d12DepthStencilFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
        d3d12DepthStencilFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

        auto d3d12DepthStencil = D3D12_DEPTH_STENCIL_DESC{};
        auto depthStencilValue = depthStencil.value_or(DepthStencilStageInfo::Default());
        d3d12DepthStencil.DepthFunc = CompareOp_to_D3D12_COMPARISON_FUNC(depthStencilValue.depthFunc);
        d3d12DepthStencil.DepthEnable = depthStencilValue.depthWrite;
        d3d12DepthStencil.StencilEnable = depthStencilValue.stencilWrite;
        d3d12DepthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        d3d12DepthStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
        d3d12DepthStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
        d3d12DepthStencil.FrontFace = d3d12DepthStencilFace;
        d3d12DepthStencil.BackFace = d3d12DepthStencilFace;

        d3d12PipelineDesc.DepthStencilState = d3d12DepthStencil;

        if (blob)
        {
            d3d12PipelineDesc.CachedPSO.pCachedBlob = blob->GetBufferPointer();
            d3d12PipelineDesc.CachedPSO.CachedBlobSizeInBytes = blob->GetBufferSize();
        }

        HRESULT result = device->CreateGraphicsPipelineState(&d3d12PipelineDesc, __uuidof(ID3D12PipelineState),
                                                             pipelineState.put_void());
        if (result == D3D12_ERROR_ADAPTER_NOT_FOUND | result == D3D12_ERROR_DRIVER_VERSION_MISMATCH |
            result == E_INVALIDARG)
        {
            d3d12PipelineDesc.CachedPSO = {};
            throwIfFailed(device->CreateGraphicsPipelineState(&d3d12PipelineDesc, __uuidof(ID3D12PipelineState),
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
        auto d3d12RootParameter = D3D12_ROOT_PARAMETER1{};
        d3d12RootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        d3d12RootParameter.Constants.ShaderRegister = 0;
        d3d12RootParameter.Constants.RegisterSpace = 0;
        d3d12RootParameter.Constants.Num32BitValues = 16;
        d3d12RootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

        std::vector<D3D12_STATIC_SAMPLER_DESC> d3d12StaticSamplerDescs;
        for (uint32_t const i : std::views::iota(0u, createInfo.staticSamplers))
        {
            auto d3d12StaticSampler = D3D12_STATIC_SAMPLER_DESC{};
            d3d12StaticSampler.Filter = Filter_to_D3D12_FILTER(createInfo.staticSamplerInfos[i].filter);
            d3d12StaticSampler.AddressU =
                AddressMode_to_D3D12_TEXTURE_ADDRESS_MODE(createInfo.staticSamplerInfos[i].addressU);
            d3d12StaticSampler.AddressV =
                AddressMode_to_D3D12_TEXTURE_ADDRESS_MODE(createInfo.staticSamplerInfos[i].addressV);
            d3d12StaticSampler.AddressW =
                AddressMode_to_D3D12_TEXTURE_ADDRESS_MODE(createInfo.staticSamplerInfos[i].addressW);
            d3d12StaticSampler.ComparisonFunc =
                CompareOp_to_D3D12_COMPARISON_FUNC(createInfo.staticSamplerInfos[i].compareOp);
            d3d12StaticSampler.MaxAnisotropy = createInfo.staticSamplerInfos[i].anisotropic;
            d3d12StaticSampler.MaxLOD = D3D12_FLOAT32_MAX;
            d3d12StaticSampler.ShaderRegister = i;
            d3d12StaticSampler.RegisterSpace = 0;
            d3d12StaticSampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

            d3d12StaticSamplerDescs.emplace_back(std::move(d3d12StaticSampler));
        }

        auto d3d12RootSignature = D3D12_VERSIONED_ROOT_SIGNATURE_DESC{};
        d3d12RootSignature.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
        d3d12RootSignature.Desc_1_1 = {.NumParameters = 1,
                                       .pParameters = &d3d12RootParameter,
                                       .NumStaticSamplers = static_cast<uint32_t>(d3d12StaticSamplerDescs.size()),
                                       .pStaticSamplers = d3d12StaticSamplerDescs.data(),
                                       .Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                                                D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED |
                                                D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED};

        winrt::com_ptr<ID3DBlob> blob;
        throwIfFailed(::D3D12SerializeVersionedRootSignature(&d3d12RootSignature, blob.put(), nullptr));
        throwIfFailed(device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(),
                                                  __uuidof(ID3D12RootSignature), rootSignature.put_void()));
    }

    auto PipelineCache::get(DX12Shader& shader, RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
                            std::optional<DepthStencilStageInfo> const depthStencil,
                            std::array<DXGI_FORMAT, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT> const& renderTargetFormats,
                            DXGI_FORMAT const depthStencilFormat) -> core::ref_ptr<Pipeline>
    {
        auto entry =
            Entry{shader.getHash(), rasterizer, blendColor, depthStencil, renderTargetFormats, depthStencilFormat};

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
        if (fence->GetCompletedValue() < fenceValue)
        {
            throwIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
            ::WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
        }
    }

    DX12GraphicsContext::DX12GraphicsContext(ID3D12Device4* device, PipelineCache& pipelineCache,
                                             DescriptorAllocator& descriptorAllocator, ID3D12CommandQueue* queue,
                                             ID3D12Fence* fence, HANDLE fenceEvent, uint64_t& fenceValue)
        : pipelineCache(&pipelineCache), descriptorAllocator(&descriptorAllocator), queue(queue), fence(fence),
          fenceEvent(fenceEvent), fenceValue(&fenceValue)
    {
        throwIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator),
                                                     commandAllocator.put_void()));

        throwIfFailed(device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE,
                                                 __uuidof(ID3D12GraphicsCommandList4), commandList.put_void()));
    }

    auto DX12GraphicsContext::reset() -> void
    {
        throwIfFailed(commandAllocator->Reset());
        throwIfFailed(commandList->Reset(commandAllocator.get(), nullptr));

        if (descriptorAllocator)
        {
            auto descriptors = std::array<ID3D12DescriptorHeap*, 2>{
                descriptorAllocator->getHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV),
                descriptorAllocator->getHeap(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER)};
            commandList->SetDescriptorHeaps(static_cast<uint32_t>(descriptors.size()), descriptors.data());
        }

        isRootSignatureBinded = false;
        currentShader = nullptr;
    }

    auto DX12GraphicsContext::setGraphicsPipelineOptions(
        core::ref_ptr<Shader> shader, RasterizerStageInfo const& rasterizer, BlendColorInfo const& blendColor,
        std::optional<DepthStencilStageInfo> const depthStencil) -> void
    {
        auto pipeline = pipelineCache->get(static_cast<DX12Shader&>(*shader), rasterizer, blendColor, depthStencil,
                                           renderTargetFormats, depthStencilFormat);

        if (!isRootSignatureBinded)
        {
            commandList->SetGraphicsRootSignature(pipeline->getRootSignature());
            isRootSignatureBinded = true;
        }

        commandList->SetPipelineState(pipeline->getPipelineState());
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        currentShader = shader;
    }

    auto DX12GraphicsContext::bindDescriptor(uint32_t const index, uint32_t const descriptor) -> void
    {
        bindings[index] = descriptor;
    }

    auto DX12GraphicsContext::beginRenderPass(std::span<RenderPassColorInfo> const colors,
                                              std::optional<RenderPassDepthStencilInfo> depthStencil) -> void
    {
        renderTargetFormats.fill(DXGI_FORMAT_UNKNOWN);
        depthStencilFormat = DXGI_FORMAT_UNKNOWN;

        auto renderPassRenderTargets =
            std::array<D3D12_RENDER_PASS_RENDER_TARGET_DESC, D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT>{};

        for (uint32_t const i : std::views::iota(0u, colors.size()))
        {
            auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS{};
            begin.Type = RenderPassLoadOp_to_D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE(colors[i].loadOp);

            begin.Clear.ClearValue.Format = TextureFormat_to_DXGI_FORMAT(colors[i].texture->getFormat());
            begin.Clear.ClearValue.Color[0] = colors[i].clearColor.r;
            begin.Clear.ClearValue.Color[1] = colors[i].clearColor.g;
            begin.Clear.ClearValue.Color[2] = colors[i].clearColor.b;
            begin.Clear.ClearValue.Color[3] = colors[i].clearColor.a;

            auto end = D3D12_RENDER_PASS_ENDING_ACCESS{};
            end.Type = RenderPassStoreOp_to_D3D12_RENDER_PASS_ENDING_ACCESS_TYPE(colors[i].storeOp);

            renderPassRenderTargets[i].BeginningAccess = begin;
            renderPassRenderTargets[i].EndingAccess = end;
            renderPassRenderTargets[i].cpuDescriptor = static_cast<DX12Texture*>(colors[i].texture.get())
                                                           ->getDescriptor(TextureUsage::RenderTarget)
                                                           .cpuHandle();

            renderTargetFormats[i] = begin.Clear.ClearValue.Format;
        }

        if (depthStencil.has_value())
        {
            auto value = depthStencil.value();
            depthStencilFormat = TextureFormat_to_DXGI_FORMAT(value.texture->getFormat());

            auto renderPassDepthStencil = D3D12_RENDER_PASS_DEPTH_STENCIL_DESC{};
            {
                auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS{};
                begin.Type = RenderPassLoadOp_to_D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE(value.depthLoadOp);
                begin.Clear.ClearValue.Format = depthStencilFormat;
                begin.Clear.ClearValue.DepthStencil.Depth = value.clearDepth;

                auto end = D3D12_RENDER_PASS_ENDING_ACCESS{};
                end.Type = RenderPassStoreOp_to_D3D12_RENDER_PASS_ENDING_ACCESS_TYPE(value.depthStoreOp);

                renderPassDepthStencil.DepthBeginningAccess = begin;
                renderPassDepthStencil.DepthEndingAccess = end;
            }

            {
                auto begin = D3D12_RENDER_PASS_BEGINNING_ACCESS{};
                begin.Type = RenderPassLoadOp_to_D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE(value.stencilLoadOp);
                begin.Clear.ClearValue.Format = depthStencilFormat;
                begin.Clear.ClearValue.DepthStencil.Depth = value.clearDepth;

                auto end = D3D12_RENDER_PASS_ENDING_ACCESS{};
                end.Type = RenderPassStoreOp_to_D3D12_RENDER_PASS_ENDING_ACCESS_TYPE(value.stencilStoreOp);

                renderPassDepthStencil.StencilBeginningAccess = begin;
                renderPassDepthStencil.StencilEndingAccess = end;
            }
            renderPassDepthStencil.cpuDescriptor =
                static_cast<DX12Texture*>(value.texture.get())->getDescriptor(TextureUsage::DepthStencil).cpuHandle();

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

    auto DX12GraphicsContext::bindVertexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset,
                                               size_t const size) -> void
    {
        auto d3d12VertexBufferView = D3D12_VERTEX_BUFFER_VIEW{};
        d3d12VertexBufferView.BufferLocation =
            static_cast<DX12Buffer*>(buffer.get())->getResource()->GetGPUVirtualAddress() + offset;
        d3d12VertexBufferView.SizeInBytes = static_cast<uint32_t>(size);
        d3d12VertexBufferView.StrideInBytes = currentShader->getInputAssembler().inputSize;

        commandList->IASetVertexBuffers(0, 1, &d3d12VertexBufferView);
    }

    auto DX12GraphicsContext::bindIndexBuffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, size_t const size,
                                              IndexFormat const format) -> void
    {
        auto d3d12IndexBufferView = D3D12_INDEX_BUFFER_VIEW{};
        d3d12IndexBufferView.BufferLocation =
            static_cast<DX12Buffer*>(buffer.get())->getResource()->GetGPUVirtualAddress() + offset;
        switch (format)
        {
            case IndexFormat::Uint32: {
                d3d12IndexBufferView.Format = DXGI_FORMAT_R32_UINT;
                break;
            }
            case IndexFormat::Uint16: {
                d3d12IndexBufferView.Format = DXGI_FORMAT_R16_UINT;
                break;
            }
        }
        d3d12IndexBufferView.SizeInBytes = size;

        commandList->IASetIndexBuffer(&d3d12IndexBufferView);
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

    auto DX12GraphicsContext::setViewport(int32_t const x, int32_t const y, uint32_t const width,
                                          uint32_t const height) -> void
    {
        auto d3d12Viewport = D3D12_VIEWPORT{};
        d3d12Viewport.TopLeftX = static_cast<float>(x);
        d3d12Viewport.TopLeftY = static_cast<float>(y);
        d3d12Viewport.Width = static_cast<float>(width);
        d3d12Viewport.Height = static_cast<float>(height);
        d3d12Viewport.MinDepth = D3D12_MIN_DEPTH;
        d3d12Viewport.MaxDepth = D3D12_MAX_DEPTH;

        commandList->RSSetViewports(1, &d3d12Viewport);
    }

    auto DX12GraphicsContext::setScissor(int32_t const left, int32_t const top, int32_t const right,
                                         int32_t const bottom) -> void
    {
        auto d3d12Rect = D3D12_RECT{};
        d3d12Rect.top = static_cast<LONG>(top);
        d3d12Rect.bottom = static_cast<LONG>(bottom);
        d3d12Rect.left = static_cast<LONG>(left);
        d3d12Rect.right = static_cast<LONG>(right);

        commandList->RSSetScissorRects(1, &d3d12Rect);
    }

    auto DX12GraphicsContext::barrier(std::variant<core::ref_ptr<Buffer>, core::ref_ptr<Texture>> dst,
                                      ResourceState const before, ResourceState const after) -> void
    {
        auto d3d12ResourceBarrier = D3D12_RESOURCE_BARRIER{};
        d3d12ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        d3d12ResourceBarrier.Transition.StateBefore = ResourceState_to_D3D12_RESOURCE_STATES(before);
        d3d12ResourceBarrier.Transition.StateAfter = ResourceState_to_D3D12_RESOURCE_STATES(after);
        d3d12ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        core::variant_match(dst)
            .case_<core::ref_ptr<Buffer>>([&](auto& data) {
                d3d12ResourceBarrier.Transition.pResource = static_cast<DX12Buffer*>(data.get())->getResource();
            })
            .case_<core::ref_ptr<Texture>>([&](auto& data) {
                d3d12ResourceBarrier.Transition.pResource = static_cast<DX12Texture*>(data.get())->getResource();
            });

        commandList->ResourceBarrier(1, &d3d12ResourceBarrier);
    }

    auto DX12GraphicsContext::execute() -> Future<Query>
    {
        throwIfFailed(commandList->Close());

        auto commandBatches =
            std::array<ID3D12CommandList*, 1>{reinterpret_cast<ID3D12CommandList*>(commandList.get())};
        queue->ExecuteCommandLists(static_cast<uint32_t>(commandBatches.size()), commandBatches.data());

        (*fenceValue)++;
        throwIfFailed(queue->Signal(fence, *fenceValue));

        auto query = core::make_ref<DX12Query>();
        auto futureImpl = std::make_unique<DX12FutureImpl>(queue, fence, fenceEvent, *fenceValue);
        return Future<Query>(query, std::move(futureImpl));
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
                                     ID3D12CommandQueue* queue, ID3D12Fence* fence, HANDLE fenceEvent,
                                     uint64_t& fenceValue)
        : device(device), queue(queue), fence(fence), fenceEvent(fenceEvent), fenceValue(&fenceValue)
    {
        throwIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, __uuidof(ID3D12CommandAllocator),
                                                     commandAllocator.put_void()));

        throwIfFailed(device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_COPY, D3D12_COMMAND_LIST_FLAG_NONE,
                                                 __uuidof(ID3D12GraphicsCommandList4), commandList.put_void()));

        {
            BufferCreateInfo bufferCreateInfo = {.size = 16 * 1024 * 1024,
                                                 .flags = (BufferUsageFlags)(BufferUsage::MapWrite)};

            bufferWrite.buffer = core::make_ref<DX12Buffer>(device, memoryAllocator, nullptr, bufferCreateInfo);
            bufferWrite.offset = 0;
        }

        {
            BufferCreateInfo bufferCreateInfo = {.size = 16 * 1024 * 1024,
                                                 .flags = (BufferUsageFlags)(BufferUsage::MapRead)};

            bufferRead.buffer = core::make_ref<DX12Buffer>(device, memoryAllocator, nullptr, bufferCreateInfo);
            bufferRead.offset = 0;
        }
    }

    auto DX12CopyContext::reset() -> void
    {
        throwIfFailed(commandAllocator->Reset());
        throwIfFailed(commandList->Reset(commandAllocator.get(), nullptr));
    }

    auto DX12CopyContext::writeBuffer(core::ref_ptr<Buffer> dst, std::span<uint8_t const> const data) -> Future<Buffer>
    {
        if (data.size() >= bufferWrite.buffer->getSize() - bufferWrite.offset)
        {
            this->execute();
            this->reset();
        }

        uint32_t const resourceAlignmentMask = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1;

        uint64_t offset = 0;
        {
            uint8_t* mappedBytes = bufferWrite.buffer->mapMemory();
            std::basic_ospanstream<uint8_t> stream(std::span<uint8_t>(mappedBytes, bufferWrite.buffer->getSize()),
                                                   std::ios::binary);
            stream.seekp(bufferWrite.offset, std::ios::beg);
            offset = stream.tellp();
            stream.write(data.data(), data.size());
            bufferWrite.offset = ((uint64_t)stream.tellp() + resourceAlignmentMask) & ~resourceAlignmentMask;
            bufferWrite.buffer->unmapMemory();
        }

        commandList->CopyBufferRegion(static_cast<DX12Buffer&>(*dst).getResource(), 0,
                                      static_cast<DX12Buffer&>(*bufferWrite.buffer).getResource(), offset, data.size());

        auto futureImpl = std::make_unique<DX12FutureImpl>(queue, fence, fenceEvent, (*fenceValue) + 1);
        return Future<Buffer>(dst, std::move(futureImpl));
    }

    auto DX12CopyContext::writeTexture(core::ref_ptr<Texture> dst,
                                       std::vector<std::span<uint8_t const>> const& data) -> Future<Texture>
    {
        size_t rowBytes = 0;
        uint32_t rowCount = 0;
        size_t totalBytes = 0;

        uint32_t const rowPitchAlignmentMask = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1;
        uint32_t const resourceAlignmentMask = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1;
        uint32_t const stride = 4;

        std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> footprints;
        footprints.resize(dst->getMipLevels());

        auto d3d12_resource_desc = static_cast<DX12Texture&>(*dst).getResource()->GetDesc();

        device->GetCopyableFootprints(&d3d12_resource_desc, 0, dst->getMipLevels(), 0, footprints.data(), nullptr,
                                      nullptr, &totalBytes);

        if (totalBytes >= bufferWrite.buffer->getSize() - bufferWrite.offset)
        {
            this->execute();
            this->reset();
        }

        {
            uint8_t* mappedBytes = bufferWrite.buffer->mapMemory();
            std::basic_ospanstream<uint8_t> stream(std::span<uint8_t>(mappedBytes, bufferWrite.buffer->getSize()),
                                                   std::ios::binary);

            stream.seekp(bufferWrite.offset, std::ios::beg);

            for (uint32_t const i : std::views::iota(0u, data.size()))
            {
                this->getSurfaceData(dst->getFormat(), footprints[i].Footprint.Width, footprints[i].Footprint.Height,
                                     rowBytes, rowCount);

                footprints[i].Footprint.RowPitch =
                    (footprints[i].Footprint.Width * stride * rowPitchAlignmentMask) & ~rowPitchAlignmentMask;
                footprints[i].Offset = stream.tellp();

                for (uint32_t const j : std::views::iota(0u, rowCount))
                {
                    stream.write(data[i].data() + rowBytes * j, rowBytes);
                    stream.seekp(footprints[i].Footprint.RowPitch - rowBytes, std::ios::cur);
                }

                size_t const totalSize = (uint64_t)stream.tellp() - footprints[i].Offset;
                stream.seekp(((totalSize + resourceAlignmentMask) & ~resourceAlignmentMask) - totalSize, std::ios::cur);
            }

            bufferWrite.offset = stream.tellp();
            bufferWrite.buffer->unmapMemory();
        }

        for (uint32_t const i : std::views::iota(0u, data.size()))
        {
            auto d3d12SourceLocation = D3D12_TEXTURE_COPY_LOCATION{};
            d3d12SourceLocation.pResource = static_cast<DX12Buffer&>(*bufferWrite.buffer).getResource();
            d3d12SourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            d3d12SourceLocation.PlacedFootprint = footprints[i];

            auto d3d12DestLocation = D3D12_TEXTURE_COPY_LOCATION{};
            d3d12DestLocation.pResource = static_cast<DX12Texture&>(*dst).getResource();
            d3d12DestLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            d3d12DestLocation.SubresourceIndex = i;

            commandList->CopyTextureRegion(&d3d12DestLocation, 0, 0, 0, &d3d12SourceLocation, nullptr);
        }

        auto futureImpl = std::make_unique<DX12FutureImpl>(queue, fence, fenceEvent, (*fenceValue) + 1);
        return Future<Texture>(dst, std::move(futureImpl));
    }

    auto DX12CopyContext::readBuffer(core::ref_ptr<Buffer> dst, std::vector<uint8_t>& data) -> void
    {
        commandList->CopyBufferRegion(static_cast<DX12Buffer&>(*bufferRead.buffer).getResource(), bufferRead.offset,
                                      static_cast<DX12Buffer&>(*dst).getResource(), 0, dst->getSize());

        Future<Query> result = this->execute();
        result.wait();
        this->reset();

        {
            uint8_t* mappedBytes = bufferRead.buffer->mapMemory();
            std::basic_ispanstream<uint8_t> stream(
                std::span<uint8_t>(mappedBytes + bufferRead.offset, bufferRead.buffer->getSize()), std::ios::binary);
            data.resize(dst->getSize());
            stream.read(data.data(), data.size());
            bufferRead.offset = stream.tellg();
            bufferRead.buffer->unmapMemory();
        }
    }

    auto DX12CopyContext::readTexture(core::ref_ptr<Texture> dst, std::vector<std::vector<uint8_t>>& data) -> void
    {
        size_t rowBytes = 0;
        uint32_t rowCount = 0;
        size_t totalBytes = 0;

        uint32_t const resourceAlignmentMask = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT - 1;

        std::vector<D3D12_PLACED_SUBRESOURCE_FOOTPRINT> footprints;
        footprints.resize(dst->getMipLevels());

        auto d3d12ResourceDesc = static_cast<DX12Texture&>(*dst).getResource()->GetDesc();

        device->GetCopyableFootprints(&d3d12ResourceDesc, 0, dst->getMipLevels(), 0, footprints.data(), nullptr,
                                      nullptr, &totalBytes);

        for (uint32_t const i : std::views::iota(0u, footprints.size()))
        {
            auto d3d12SourceLocation = D3D12_TEXTURE_COPY_LOCATION{};
            d3d12SourceLocation.pResource = static_cast<DX12Texture&>(*dst).getResource();
            d3d12SourceLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            d3d12SourceLocation.SubresourceIndex = i;

            auto d3d12_dest_location = D3D12_TEXTURE_COPY_LOCATION{};
            d3d12_dest_location.pResource = static_cast<DX12Buffer&>(*bufferRead.buffer).getResource();
            d3d12_dest_location.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
            d3d12_dest_location.PlacedFootprint = footprints[i];

            commandList->CopyTextureRegion(&d3d12_dest_location, 0, 0, 0, &d3d12SourceLocation, nullptr);
        }

        Future<Query> result = this->execute();
        result.wait();
        this->reset();

        {
            uint8_t* mappedBytes = bufferRead.buffer->mapMemory();
            std::basic_ispanstream<uint8_t> stream(
                std::span<uint8_t>(mappedBytes + bufferRead.offset, bufferRead.buffer->getSize()), std::ios::binary);

            data.resize(footprints.size());

            for (uint32_t const i : std::views::iota(0u, footprints.size()))
            {
                this->getSurfaceData(dst->getFormat(), footprints[i].Footprint.Width, footprints[i].Footprint.Height,
                                     rowBytes, rowCount);

                data[i].resize(rowBytes * rowCount);

                for (uint32_t const j : std::views::iota(0u, rowCount))
                {
                    stream.read(data[i].data() + rowBytes * j, rowBytes);
                    stream.seekg(footprints[i].Footprint.RowPitch - rowBytes, std::ios::cur);
                }

                size_t const totalSize = (uint64_t)stream.tellg() - footprints[i].Offset;
                stream.seekg(((totalSize + resourceAlignmentMask) & ~resourceAlignmentMask) - totalSize, std::ios::cur);
            }

            bufferRead.offset = stream.tellg();
            bufferRead.buffer->unmapMemory();
        }
    }

    auto DX12CopyContext::barrier(std::variant<core::ref_ptr<Buffer>, core::ref_ptr<Texture>> dst,
                                  ResourceState const before, ResourceState const after) -> void
    {
        auto d3d12ResourceBarrier = D3D12_RESOURCE_BARRIER{};
        d3d12ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        d3d12ResourceBarrier.Transition.StateBefore = ResourceState_to_D3D12_RESOURCE_STATES(before);
        d3d12ResourceBarrier.Transition.StateAfter = ResourceState_to_D3D12_RESOURCE_STATES(after);
        d3d12ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        core::variant_match(dst)
            .case_<core::ref_ptr<Buffer>>([&](auto& data) {
                d3d12ResourceBarrier.Transition.pResource = static_cast<DX12Buffer*>(data.get())->getResource();
            })
            .case_<core::ref_ptr<Texture>>([&](auto& data) {
                d3d12ResourceBarrier.Transition.pResource = static_cast<DX12Texture*>(data.get())->getResource();
            });

        commandList->ResourceBarrier(1, &d3d12ResourceBarrier);
    }

    auto DX12CopyContext::execute() -> Future<Query>
    {
        bufferWrite.offset = 0;
        bufferRead.offset = 0;

        throwIfFailed(commandList->Close());

        auto commandBatches =
            std::array<ID3D12CommandList*, 1>{reinterpret_cast<ID3D12CommandList*>(commandList.get())};

        queue->ExecuteCommandLists(static_cast<uint32_t>(commandBatches.size()), commandBatches.data());
        (*fenceValue)++;
        throwIfFailed(queue->Signal(fence, *fenceValue));

        auto query = core::make_ref<DX12Query>();
        auto futureImpl = std::make_unique<DX12FutureImpl>(queue, fence, fenceEvent, *fenceValue);
        return Future<Query>(query, std::move(futureImpl));
    }

    DX12Device::DX12Device(RHICreateInfo const& createInfo)
    {
#ifdef _DEBUG
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
                auto dxgi_adapter_desc = DXGI_ADAPTER_DESC1{};
                throwIfFailed(adapter->GetDesc1(&dxgi_adapter_desc));

                if (dxgi_adapter_desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
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

        fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (!fenceEvent)
        {
            throwIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }

        // Create Direct Queue (Default Queue)
        {
            auto d3d12QueueDesc = D3D12_COMMAND_QUEUE_DESC{};
            d3d12QueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
            d3d12QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

            throwIfFailed(device->CreateCommandQueue(&d3d12QueueDesc, __uuidof(ID3D12CommandQueue),
                                                     graphicsQueue.queue.put_void()));
        }

        throwIfFailed(
            device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), graphicsQueue.fence.put_void()));
        graphicsQueue.fenceValue = 0;

        // Create Copy Queue
        {
            auto d3d12QueueDesc = D3D12_COMMAND_QUEUE_DESC{};
            d3d12QueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
            d3d12QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

            throwIfFailed(
                device->CreateCommandQueue(&d3d12QueueDesc, __uuidof(ID3D12CommandQueue), copyQueue.queue.put_void()));
        }

        throwIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), copyQueue.fence.put_void()));
        copyQueue.fenceValue = 0;

        // Create Compute Queue
        {
            auto d3d12QueueDesc = D3D12_COMMAND_QUEUE_DESC{};
            d3d12QueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
            d3d12QueueDesc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

            throwIfFailed(device->CreateCommandQueue(&d3d12QueueDesc, __uuidof(ID3D12CommandQueue),
                                                     computeQueue.queue.put_void()));
        }

        throwIfFailed(
            device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), computeQueue.fence.put_void()));
        computeQueue.fenceValue = 0;

        descriptorAllocator = core::make_ref<DescriptorAllocator>(device.get());

        auto d3d12maAllocatorDesc = D3D12MA::ALLOCATOR_DESC{};
        d3d12maAllocatorDesc.pDevice = device.get();
        d3d12maAllocatorDesc.pAdapter = adapter.get();

        throwIfFailed(D3D12MA::CreateAllocator(&d3d12maAllocatorDesc, memoryAllocator.put()));

        pipelineCache = core::make_ref<PipelineCache>(device.get(), createInfo);

        if (createInfo.targetWindow)
        {
            {
                auto dxgiSwapchainDesc = DXGI_SWAP_CHAIN_DESC1{};
                dxgiSwapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
                dxgiSwapchainDesc.Width = createInfo.windowWidth;
                dxgiSwapchainDesc.Height = createInfo.windowHeight;
                dxgiSwapchainDesc.BufferCount = 2;
                dxgiSwapchainDesc.SampleDesc.Count = 1;
                dxgiSwapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                dxgiSwapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
                dxgiSwapchainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

                winrt::com_ptr<IDXGISwapChain1> swapchain;
                throwIfFailed(factory->CreateSwapChainForHwnd(graphicsQueue.queue.get(),
                                                              reinterpret_cast<HWND>(createInfo.targetWindow),
                                                              &dxgiSwapchainDesc, nullptr, nullptr, swapchain.put()));
                swapchain.as(this->swapchain);
            }

            for (uint32_t const i : std::views::iota(0u, 2u))
            {
                core::ref_ptr<Texture> texture;
                {
                    winrt::com_ptr<ID3D12Resource> resource;
                    throwIfFailed(swapchain->GetBuffer(i, __uuidof(ID3D12Resource), resource.put_void()));

                    texture = core::make_ref<DX12Texture>(device.get(), resource, *descriptorAllocator);
                }
                backBuffers.emplace_back(texture);
            }
        }
    }

    DX12Device::~DX12Device()
    {
        ::CloseHandle(fenceEvent);
    }

    auto DX12Device::createShader(ShaderCreateInfo const& createInfo) -> core::ref_ptr<Shader>
    {
        std::lock_guard lock(mutex);
        auto shader = core::make_ref<DX12Shader>(device.get(), vertexDeclarations, vertexShader, pixelShader);
        return shader;
    }

    auto DX12Device::createTexture(TextureCreateInfo const& createInfo) -> core::ref_ptr<Texture>
    {
        std::lock_guard lock(mutex);

        auto texture =
            core::make_ref<DX12Texture>(device.get(), memoryAllocator.get(), *descriptorAllocator, createInfo);
        return texture;
    }

    auto DX12Device::createBuffer(BufferCreateInfo const& createInfo) -> core::ref_ptr<Buffer>
    {
        std::lock_guard lock(mutex);

        auto buffer =
            core::make_ref<DX12Buffer>(device.get(), memoryAllocator.get(), descriptorAllocator.get(), createInfo);
        return buffer;
    }

    auto DX12Device::createGraphicsContext() -> core::ref_ptr<GraphicsContext>
    {
        std::lock_guard lock(mutex);

        auto context = core::make_ref<DX12GraphicsContext>(device.get(), *pipelineCache, *descriptorAllocator,
                                                           graphicsQueue.queue.get(), graphicsQueue.fence.get(),
                                                           fenceEvent, graphicsQueue.fenceValue);
        return context;
    }

    auto DX12Device::createCopyContext() -> core::ref_ptr<CopyContext>
    {
        std::lock_guard lock(mutex);

        auto context = core::make_ref<DX12CopyContext>(device.get(), memoryAllocator.get(), copyQueue.queue.get(),
                                                       copyQueue.fence.get(), fenceEvent, copyQueue.fenceValue);
        return context;
    }

    auto DX12Device::requestBackBuffer() -> core::ref_ptr<Texture>
    {
        if (!swapchain)
        {
            throw core::Exception("Swapchain is not found");
        }

        if (graphicsQueue.fence->GetCompletedValue() < graphicsQueue.fenceValue)
        {
            throwIfFailed(graphicsQueue.fence->SetEventOnCompletion(graphicsQueue.fenceValue, fenceEvent));
            ::WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
        }
        return backBuffers[swapchain->GetCurrentBackBufferIndex()];
    }

    auto DX12Device::presentBackBuffer() -> void
    {
        if (!swapchain)
        {
            throw core::Exception("Swapchain is not found");
        }

        throwIfFailed(swapchain->Present(0, 0));

        graphicsQueue.fenceValue++;
        throwIfFailed(graphicsQueue.queue->Signal(graphicsQueue.fence.get(), graphicsQueue.fenceValue));
    }

    auto DX12Device::getBackendType() const -> std::string_view
    {
        return "D3D12";
    }
} // namespace ionengine::rhi