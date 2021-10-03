// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

using namespace memory_literals;

template<enum D3D12_HEAP_TYPE T, usize S>
class D3DMemoryPool {
public:

    struct Key {
        uint32 alignment;
        D3D12_HEAP_FLAGS heap_flags;

        bool operator<(const Key& rhs) const {
            return std::tie(alignment, heap_flags) < std::tie(rhs.alignment, rhs.heap_flags);
        }
    };

    D3DMemoryPool(ID3D12Device4* d3d12_device, const usize default_heap_size) 
        : m_d3d12_device(d3d12_device), m_default_heap_size(default_heap_size) {

    }

    [[nodiscard]] D3DMemoryPtr allocate(const usize size, const uint32 alignment, const D3D12_HEAP_FLAGS heap_flags) {

        Key key = {
            alignment,
            heap_flags
        };

        D3DMemoryPtr ptr{};

        std::cout << "size to alloc " << size << std::endl;
        usize align_size = aligned_block_size(size);
        std::cout << "align size to alloc " << align_size << std::endl;

        auto it_heap = m_memory_heaps.find(key);
        if(it_heap != m_memory_heaps.end()) {
            for(auto& heap : it_heap->second) {

                if(heap.offset + align_size > heap.heap_size) {
                    continue;
                } else {
                    usize alloc_size = 0;
                    for(uint64 i = heap.offset / heap.block_size; i < heap.block_data.size(); ++i) {
                        if(alloc_size == align_size) {
                            std::memset(heap.block_data.data() + ptr.offset / m_block_size, 0x1, sizeof(uint8) * align_size / m_block_size);
                            std::cout << "memory pool free block found" << std::endl;
                            break;
                        }

                        if(alloc_size == 0) {
                            ptr.heap = &heap;
                            ptr.offset = i * heap.block_size;
                        }

                        alloc_size = heap.block_data[i] == 0x0 ? alloc_size + heap.block_size : alloc_size;
                    } 
                    if(alloc_size != align_size) {
                        ptr.heap = nullptr;
                        ptr.offset = 0;

                        std::cout << "memory pool free block not found" << std::endl;
                    }
                }
            }
        }
        if(!ptr.heap) {
            assert((align_size + alignment < m_default_heap_size) && "allocation size should be less than default heap size");

            auto d3d12_heap = create_heap(m_default_heap_size, alignment, heap_flags);
            auto& heap = m_memory_heaps[key].emplace_back(D3DMemoryHeap { d3d12_heap, m_default_heap_size, m_default_heap_size / m_block_size, m_block_size });

            heap.block_data.resize(m_default_heap_size / m_block_size);
            std::memset(heap.block_data.data(), 0x0, sizeof(uint8) * align_size / m_block_size);

            ptr.heap = &heap;
            ptr.offset = heap.offset;

            std::memset(heap.block_data.data() + ptr.offset / m_block_size, 0x1, sizeof(uint8) * align_size / m_block_size); 
            heap.offset += align_size;

            std::cout << "memory pool allocating new heap" << std::endl;
        } 
        return ptr;
    }

    void deallocate(const D3DMemoryPtr& ptr, const usize size) {
        
        usize align_size = aligned_block_size(size);

        std::memset(ptr.heap->block_data.data() + ptr.offset / m_block_size, 0x0, sizeof(uint8) * align_size / m_block_size);
        ptr.heap->offset = ptr.offset;

        std::cout << "memory pool deallocate memory" << std::endl;
    }

private:

    ID3D12Device4* m_d3d12_device;

    winrt::com_ptr<ID3D12Heap> create_heap(const usize size, const uint32 alignment, const D3D12_HEAP_FLAGS heap_flags) {
        winrt::com_ptr<ID3D12Heap> d3d12_heap;
        
        D3D12_HEAP_DESC heap_desc{};
        heap_desc.Properties.Type = m_heap_type;
        heap_desc.SizeInBytes = size;
        heap_desc.Alignment = alignment;
        heap_desc.Flags = heap_flags;
        
        THROW_IF_FAILED(m_d3d12_device->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), d3d12_heap.put_void()));
        return d3d12_heap;
    }

    usize aligned_block_size(const usize size) const {
        return size < m_block_size ? 
            m_block_size : (size % m_block_size) > 0 ?
                ((size / m_block_size) + 1) * m_block_size : size;
    }

    std::map<D3DMemoryPool<T, S>::Key, std::list<D3DMemoryHeap>> m_memory_heaps;

    usize m_default_heap_size;

    const D3D12_HEAP_TYPE m_heap_type = T;
    const usize m_block_size = S;
};

class D3DMemoryAllocatorWrapper {
public:

    static void initialize(
        ID3D12Device4* d3d12_device, 
        const usize buffers_pool_size, 
        const usize textures_pool_size,
        const usize rtds_pool_size
    ) {
        m_buffers_memory_pool = std::make_unique<D3DMemoryPool<D3D12_HEAP_TYPE_DEFAULT, 64_kb>>(d3d12_device, buffers_pool_size);
        m_textures_memory_pool = std::make_unique<D3DMemoryPool<D3D12_HEAP_TYPE_DEFAULT, 1_mb>>(d3d12_device, textures_pool_size);
        m_rtds_textures_memory_pool = std::make_unique<D3DMemoryPool<D3D12_HEAP_TYPE_DEFAULT, 1_mb>>(d3d12_device, rtds_pool_size);
    }

    [[nodiscard]] static D3DMemoryPtr allocate(const ResourceType resource_type, const MemoryType memory_type, const usize size, const uint32 alignment, const ResourceFlags resource_flags) {
        
        D3D12_HEAP_FLAGS heap_flags;
        switch(resource_flags) {
            case ResourceFlags::ConstantBuffer:
            case ResourceFlags::IndexBuffer:
            case ResourceFlags::VertexBuffer:
            case ResourceFlags::UnorderedAccess: heap_flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS; break;
            case ResourceFlags::ShaderResource: heap_flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES; break;
            case ResourceFlags::DepthStencil:
            case ResourceFlags::RenderTarget: heap_flags = D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES; break;
            default: heap_flags = D3D12_HEAP_FLAG_NONE;
        }

        D3DMemoryPtr ptr{};
        switch(resource_type) {
            case ResourceType::Buffer: {
                ptr = m_buffers_memory_pool->allocate(size, alignment, heap_flags);
                break;
            }
            case ResourceType::Texture: {
                if(resource_flags & (ResourceFlags::RenderTarget | ResourceFlags::DepthStencil)) {
                    ptr = m_rtds_textures_memory_pool->allocate(size, alignment, heap_flags);
                } else {
                    ptr = m_textures_memory_pool->allocate(size, alignment, heap_flags);
                }
                break;
            }
        }
        return ptr;
    }

    static void deallocate(const ResourceType resource_type, const D3DMemoryPtr& ptr, const usize size, const ResourceFlags resource_flags) {

        switch(resource_type) {
            case ResourceType::Buffer: {
                m_buffers_memory_pool->deallocate(ptr, size);
                break;
            }
            case ResourceType::Texture: {
                if(resource_flags & (ResourceFlags::RenderTarget | ResourceFlags::DepthStencil)) {
                    m_rtds_textures_memory_pool->deallocate(ptr, size);
                } else {
                    m_textures_memory_pool->deallocate(ptr, size);
                }
                break;
            }
        }
    }

private:

    inline static std::unique_ptr<D3DMemoryPool<D3D12_HEAP_TYPE_DEFAULT, 64_kb>> m_buffers_memory_pool;
    inline static std::unique_ptr<D3DMemoryPool<D3D12_HEAP_TYPE_DEFAULT, 1_mb>> m_textures_memory_pool;
    inline static std::unique_ptr<D3DMemoryPool<D3D12_HEAP_TYPE_DEFAULT, 1_mb>> m_rtds_textures_memory_pool;
};

}