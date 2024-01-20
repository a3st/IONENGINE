// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/rhi/buffer.hpp"

namespace ionengine {

namespace renderer {

namespace rhi {

class DX12Buffer : public Buffer {
public:

    DX12Buffer();

    auto get_size() -> size_t override {

        return 0;
    }

    auto get_flags() -> BufferUsageFlags override {

        return flags;
    }

protected:

    BufferUsageFlags flags;
};

}

}

}