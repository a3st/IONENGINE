// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib::hash {

class BufferView {
public:

    BufferView(std::span<uint8_t const> const data);

    uint64_t hash() const;

    uint8_t const* data() const;

    size_t size() const;

private:
    
    std::span<uint8_t const> _data;
    uint64_t _data_hash{0};
    uint32_t _count{0};
};

}
