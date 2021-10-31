// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class CommandBuffer {

friend class Device;

public:

    CommandBuffer(Device* device, const CommandBufferType type);
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&&) = delete;

    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&&) = delete;

    void BindPipeline(Pipeline* pipeline);
    void BindDescriptorSet(DescriptorSet* set);
    void SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
    void SetScissorRect(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom);
    void BeginRenderPass(RenderPass* render_pass, FrameBuffer* frame_buffer, const std::span<ClearValueColor>& colors, float depth, uint8_t stencil);
    void EndRenderPass();
    void TextureMemoryBarrier(Texture* texture, const MemoryState before, const MemoryState after);
    void Reset();
    void Close();
    void DrawInstanced(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);
    void DrawIndexed(const uint32_t index_count, const uint32_t instance_count, const uint32_t first_index, const uint32_t vertex_offset, const uint32_t first_instance);
    void SetIndexBuffer(BufferView* buffer_view);
    void SetVertexBuffer(const uint32_t slot, BufferView* buffer_view);

private:

    Device* device_;

    ComPtr<ID3D12CommandAllocator> allocator_;
    ComPtr<ID3D12GraphicsCommandList4> list_;
};

}