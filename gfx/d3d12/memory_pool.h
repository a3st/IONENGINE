// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

using namespace memory_literals;

struct D3DMemoryHeap {
    winrt::com_ptr<ID3D12Heap> d3d12_heap;
    usize heap_size;
    uint64 block_count;
    usize block_size;
    uint64 offset;
    std::vector<uint8> block_data;
};

struct D3DMemoryPtr {
    D3DMemoryHeap* heap;
    uint64 offset;
};

template<usize T>
class D3DMemoryPool {
public:

    struct Key {
        MemoryType memory_type;
        uint32 alignment;
        ResourceFlags resource_flags;

        bool operator<(const Key& rhs) const {
            return std::tie(memory_type, alignment, resource_flags) < std::tie(rhs.memory_type, rhs.alignment, rhs.resource_flags);
        }
    };

    D3DMemoryPool(ID3D12Device4* d3d12_device, const usize default_size) 
        : m_d3d12_device(d3d12_device), m_default_heap_size(default_size) {

    }

    [[nodiscard]] D3DMemoryPtr allocate(const MemoryType memory_type, const usize size, const uint32 alignment, const ResourceFlags resource_flags) {

        Key key = {
            memory_type,
            alignment,
            resource_flags
        };

        D3DMemoryPtr ptr{};

        std::cout << "size to alloc " << size << std::endl;
        usize align_size = aligned_block_size(size);
        std::cout << "align size to alloc " << align_size << std::endl;

        auto it_heap = m_memory_heaps.find(key);
        if(it_heap != m_memory_heaps.end()) {
            for(auto& heap : it_heap->second) {

                usize alloc_size = 0;
                if(heap.offset + align_size > heap.heap_size) {
                    continue;
                } else {
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

                        if(!heap.block_data[i]) {
                            alloc_size += heap.block_size;
                        } else {
                            alloc_size = 0;
                        }
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

            auto d3d12_heap = create_heap(memory_type, m_default_heap_size, alignment, resource_flags);
            auto& heap = m_memory_heaps[key].emplace_back(D3DMemoryHeap { d3d12_heap, m_default_heap_size, m_default_heap_size / m_block_size, m_block_size });

            heap.block_data.resize(m_default_heap_size / m_block_size);

            std::memset(heap.block_data.data() + heap.offset / m_block_size, 0x0, sizeof(uint8) * align_size / m_block_size);

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

    void debug_test() {

        std::cout << "--------------------------" << std::endl;
        for(auto& heap_type : m_memory_heaps) {

            for(auto& heap : heap_type.second) {
            
                std::cout << format<char>("Heap (Size: {}, BlockCount: {})", heap.heap_size, heap.block_count) << std::endl;
                for(uint64 i = 0; i < heap.block_data.size(); ++i) {
                    if(heap.block_data[i] == 0x1) {
                        std::cout << "1";
                    } else {
                        std::cout << "0";
                    }
                }
                std::cout << std::endl;
            }
        }
        std::cout << "--------------------------" << std::endl;
    }

private:

    ID3D12Device4* m_d3d12_device;

    winrt::com_ptr<ID3D12Heap> create_heap(const MemoryType memory_type, const usize size, const uint32 alignment, const ResourceFlags resource_flags) {
        winrt::com_ptr<ID3D12Heap> d3d12_heap;
        
        D3D12_HEAP_DESC heap_desc{};
        heap_desc.Properties.Type = d3d12_heap_type_to_gfx_enum(memory_type);
        heap_desc.SizeInBytes = size;
        heap_desc.Alignment = alignment;
        switch(resource_flags) {
            case ResourceFlags::ConstantBuffer:
            case ResourceFlags::IndexBuffer:
            case ResourceFlags::VertexBuffer:
            case ResourceFlags::UnorderedAccess: heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_BUFFERS; break;
            case ResourceFlags::ShaderResource: heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_NON_RT_DS_TEXTURES; break;
            case ResourceFlags::DepthStencil:
            case ResourceFlags::RenderTarget: heap_desc.Flags = D3D12_HEAP_FLAG_ALLOW_ONLY_RT_DS_TEXTURES; break;
        }
        THROW_IF_FAILED(m_d3d12_device->CreateHeap(&heap_desc, __uuidof(ID3D12Heap), d3d12_heap.put_void()));
        return d3d12_heap;
    }

    usize aligned_block_size(const usize size) const {
        return size < m_block_size ? 
            m_block_size : (size % m_block_size) > 0 ?
                ((size / m_block_size) + 1) * m_block_size : size;
    }

    std::map<D3DMemoryPool<T>::Key, std::list<D3DMemoryHeap>> m_memory_heaps;

    usize m_default_heap_size;
    const usize m_block_size = T;
};

class D3DAllocatorWrapper {
public:

    static void initialize(
        ID3D12Device4* d3d12_device, 
        const usize buffers_pool_size, 
        const usize textures_pool_size,
        const usize rtds_pool_size
    ) {
        m_buffers_memory_pool = std::make_unique<D3DMemoryPool<64_kb>>(d3d12_device, buffers_pool_size);
        m_textures_memory_pool = std::make_unique<D3DMemoryPool<1_mb>>(d3d12_device, textures_pool_size);
        m_rtds_textures_memory_pool = std::make_unique<D3DMemoryPool<1_mb>>(d3d12_device, rtds_pool_size);
    }

    [[nodiscard]] static D3DMemoryPtr allocate(const ResourceType resource_type, const MemoryType memory_type, const usize size, const uint32 alignment, const ResourceFlags resource_flags) {
        
        D3DMemoryPtr memory_ptr;
        switch(resource_type) {
            case ResourceType::Buffer: {
                memory_ptr = m_buffers_memory_pool->allocate(memory_type, size, alignment, resource_flags);
                break;
            }
            case ResourceType::Texture: {
                // todo
                break;
            }
        }
        return memory_ptr;
    }

    static void deallocate(const ResourceType resource_type, const D3DMemoryPtr& ptr, const usize size) {

        switch(resource_type) {
            case ResourceType::Buffer: {
                m_buffers_memory_pool->deallocate(ptr, size);
                break;
            }
            case ResourceType::Texture: {
                // todo
                break;
            }
        }
    }

    static void debug_test() {
        m_buffers_memory_pool->debug_test();
    }

private:

    inline static std::unique_ptr<D3DMemoryPool<64_kb>> m_buffers_memory_pool;
    inline static std::unique_ptr<D3DMemoryPool<1_mb>> m_textures_memory_pool;
    inline static std::unique_ptr<D3DMemoryPool<1_mb>> m_rtds_textures_memory_pool;
};

}