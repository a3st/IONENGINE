// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../../lgfx/lgfx.h"

namespace ionengine::rendersystem {

class BufferViewCache {

public:

    struct Key {

        lgfx::Buffer* buffer;
        lgfx::Format index_format;
        uint32_t stride;

        inline bool operator<(const Key& rhs) const {

            return std::tie(buffer, index_format, stride) < std::tie(rhs.buffer, rhs.index_format, rhs.stride);
        }
    };

    BufferViewCache(lgfx::Device* device);
    BufferViewCache(const BufferViewCache&) = delete;
    BufferViewCache(BufferViewCache&&) = delete;

    BufferViewCache& operator=(const BufferViewCache&) = delete;
    BufferViewCache& operator=(BufferViewCache&&) = delete;

    lgfx::BufferView* GetBufferView(lgfx::Buffer* buffer, const lgfx::Format index_format, const uint32_t stride);

    inline void Reset() {

        buffer_views_.clear();
    }

private:

    lgfx::Device* device_;

    lgfx::DescriptorPool sr_descriptor_pool_;

    std::map<Key, std::unique_ptr<lgfx::BufferView>> buffer_views_;
};

}