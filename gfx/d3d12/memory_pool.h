// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

struct D3DMemoryPtr {
    ID3D12Heap* d3d12_heap;
    uint64 offset;
};

class D3DMemoryPool : public MemoryPool {
public:

    D3DMemoryPool(ID3D12Device4* d3d12_device, const usize default_size, const usize default_block_size) 
        : m_d3d12_device(d3d12_device), m_default_size(default_size), m_default_block_size(default_block_size) {

    }

    [[nodiscard]] D3DMemoryPtr allocate(const MemoryType memory_type, const usize size, const uint32 alignment, const ResourceFlags resource_flags) {

        Key key = {
            memory_type,
            size,
            alignment,
            resource_flags
        };

        D3DMemoryPtr ptr{};

        auto it_heap = m_memory_sizes.find(key);
        if(it_heap != m_memory_sizes.end()) {

            auto it_block = m_free_blocks.find(size);
            if(it_block != m_free_blocks.end()) {
                
                if(!it_block->second.empty()) {
                    ptr = it_block->second.back();
                    it_block->second.pop();

                    std::cout << "memory pool get free block" << std::endl;
                    return ptr;
                }
            }
            
            if(it_heap->second->offset + size + alignment > it_heap->second->heap_size) {
                assert((size + alignment < m_default_size) && "allocation size should be less than default size pool");
                auto d3d12_heap = create_heap(memory_type, m_default_size, alignment, resource_flags);
                m_memory_heaps.emplace_back(D3DMemoryHeap { d3d12_heap, m_default_size, m_default_size / m_default_block_size, m_default_block_size, 0 });

                auto& last_heap = std::prev(m_memory_heaps.end());
                ptr.d3d12_heap = last_heap->d3d12_heap.get();
                ptr.offset = last_heap->offset;

                last_heap->offset += size;

                m_memory_sizes[key] = last_heap;

                std::cout << "memory pool offset size > heap_size" << std::endl;
            } else {
                ptr.d3d12_heap = it_heap->second->d3d12_heap.get();
                ptr.offset = it_heap->second->offset;

                it_heap->second->offset += size;

                std::cout << "memory pool get from pool memory" << std::endl;
            }
        } else {
            assert((size + alignment < m_default_size) && "allocation size should be less than default size pool");
            auto d3d12_heap = create_heap(memory_type, m_default_size, alignment, resource_flags);
            m_memory_heaps.emplace_back(D3DMemoryHeap { d3d12_heap, m_default_size, m_default_size / m_default_block_size, m_default_block_size, 0 });
            
            auto& last_heap = std::prev(m_memory_heaps.end());
            ptr.d3d12_heap = last_heap->d3d12_heap.get();
            ptr.offset = last_heap->offset;

            std::cout << "size to alloc " << size << std::endl;
            usize align_memory = (size % m_default_block_size) > 0 ? ((size / m_default_block_size) + 1) * m_default_block_size : size;
            if(size < m_default_block_size) {
                align_memory = m_default_block_size;
            }

            std::cout << "need size to alloc " << align_memory << std::endl;
            last_heap->offset += size;

            m_memory_sizes[key] = last_heap;

            std::cout << "memory pool allocating new heap" << std::endl;
        }
        return ptr;
    }

    void deallocate(const D3DMemoryPtr& ptr, const usize size) {
        m_free_blocks[size].push(ptr);

        std::cout << "memory pool deallocate memory" << std::endl;
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

    struct D3DMemoryHeap {
        winrt::com_ptr<ID3D12Heap> d3d12_heap;
        usize heap_size;
        uint64 block_count;
        usize block_size;
        uint64 offset;
    };

    std::list<D3DMemoryHeap> m_memory_heaps;

    struct Key {
        MemoryType memory_type;
        usize size;
        uint32 alignment;
        ResourceFlags resource_flags;

        bool operator<(const Key& rhs) const {
            return std::tie(memory_type, size, alignment, resource_flags) < std::tie(rhs.memory_type, rhs.size, rhs.alignment, rhs.resource_flags);
        }
    };

    using D3DMemoryHeapIt = std::list<D3DMemoryHeap>::iterator;
    std::map<Key, D3DMemoryHeapIt> m_memory_sizes;
    std::map<usize, std::queue<D3DMemoryPtr>> m_free_blocks;

    usize m_default_size;
    usize m_default_block_size;
};

}