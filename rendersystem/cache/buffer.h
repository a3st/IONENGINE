// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class BufferCache {

public:

    struct Key {

        size_t size;
        lgfx::BufferFlags flags;

        inline bool operator<(const Key& rhs) const { 
            
            return std::tie(size, flags) < std::tie(rhs.size, rhs.flags);
        }
    };

    BufferCache(lgfx::Device* device);
    BufferCache(const BufferCache&) = delete;
    BufferCache(BufferCache&&) = delete;

    BufferCache& operator=(const BufferCache&) = delete;
    BufferCache& operator=(BufferCache&&) = delete;

    lgfx::Buffer* GetBuffer(const size_t size, const lgfx::BufferFlags flags);
    void Clear();

    inline void Reset() {

        buffers_.clear();
    }

private:

    lgfx::Device* device_;

    lgfx::MemoryPool memory_pool_;

    struct BufferEntry {
        uint32_t entry_index;
        std::vector<std::unique_ptr<lgfx::Buffer>> buffers;
    };

    std::map<Key, BufferEntry> buffers_;
};

}