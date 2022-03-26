// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

template<size_t TotalSize, size_t ChunkSize>
class PoolAllocator {

    static_assert(ChunkSize >= 8 && "Chunk size must be greater or equal to 8");
    static_assert(TotalSize % ChunkSize == 0 && "Total Size must be a multiple of Chunk size");

public:

    PoolAllocator() {

        _ptr = std::malloc(TotalSize);
        fill_free_data();
    }

    PoolAllocator(PoolAllocator const&) = delete;

    PoolAllocator(PoolAllocator&&) noexcept = delete;

    PoolAllocator& operator=(PoolAllocator const&) = delete;

    PoolAllocator& operator=(PoolAllocator&&) = delete;

    ~PoolAllocator() {

        free(_ptr);
    }

    void* allocate(size_t const size, size_t const align) {

        assert(!_free_data.empty() && "The pool allocator is full");

        Chunk* ptr = _free_data.front();
        _free_data.pop_front();
        return reinterpret_cast<void*>(ptr);
    }

    void deallocate(void* ptr) {

        _free_data.emplace_back(reinterpret_cast<Chunk*>(ptr));
    }

    void reset() {

        fill_free_data();
    }

    size_t max_size() const { 
        
        return TotalSize / ChunkSize;
    }

private:

    struct Chunk;
    
    std::list<Chunk*> _free_data;

    void* _ptr{nullptr};

    void fill_free_data() {

        uint32_t const chunk_count = TotalSize / ChunkSize;

        for(uint32_t i = 0; i < chunk_count; ++i) {
            
            uint64_t const address = reinterpret_cast<uint64_t>(_ptr) + i * ChunkSize;
            _free_data.emplace_back(reinterpret_cast<Chunk*>(address));
        }
    }
};

}
