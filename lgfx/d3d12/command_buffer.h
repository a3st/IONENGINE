// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class CommandBuffer {

friend class Device;

public:

    CommandBuffer(Device* device, const CommandBufferType type);

    void SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
    void SetScissorRect(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom);
    void BeginRenderPass(RenderPass* render_pass, FrameBuffer* frame_buffer, const ClearValueDesc& desc);
    void EndRenderPass();
    void TextureMemoryBarrier(Texture* texture, const MemoryState before, const MemoryState after);
    void Reset();
    void Close();
    
private:

    Device* device_;

    ComPtr<ID3D12CommandAllocator> allocator_;
    ComPtr<ID3D12GraphicsCommandList4> list_;
};

}