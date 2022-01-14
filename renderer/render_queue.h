// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/color.h>

namespace ionengine::renderer {

enum class RenderDataType {
    Barrier,
    RenderPass
};

enum class BarrierType {
    Present,
    RenderTarget
};

enum class RenderPassLoadOp {
    Load,
    Clear,
    DontCare
};

enum class RenderPassStoreOp {
    Store,
    DontCare
};

struct BarrierData {
    GPUResourceHandle resource;
    BarrierType before;
    BarrierType after;
};

struct RenderPassColorDesc {
    RenderPassLoadOp load_op;
    RenderPassStoreOp store_op;
};

struct RenderPassDepthStencilDesc {
    RenderPassLoadOp depth_load_op;
    RenderPassStoreOp depth_store_op;
    RenderPassLoadOp stencil_load_op;
    RenderPassStoreOp stencil_store_op;
};

struct RenderPassData {
    std::array<GPUResourceHandle, 8> rtvs;
    std::array<RenderPassColorDesc, 8> rtv_ops;
    std::array<Color, 8> rtv_clears;
    uint16_t rtv_count;
    GPUResourceHandle dsv;
    RenderPassDepthStencilDesc dsv_op;
    std::pair<float, uint8_t> dsv_clear;
};

struct RenderData {
    RenderDataType render_type;
    std::variant<BarrierData, RenderPassData> data;
};

class RenderQueue {
public:

    RenderQueue() = default;

    void push(RenderData const& element) {

        _array.emplace_back(element);
        ++_offset;
    }

    void pop(RenderData& element) {

        --_offset;
        element = _array[_offset];
    }

    bool empty() {
        
        return _offset == 0;
    }

private:

    //enum { Capacity = Size + 1 };

    size_t _offset{0};
    std::vector<RenderData> _array;
};

}