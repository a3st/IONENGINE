// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"

namespace lgfx {

class CommandBuffer {

friend class Device;

public:

    CommandBuffer(Device* const device, const CommandBufferType type);
    CommandBuffer(const CommandBuffer&) = delete;
    CommandBuffer(CommandBuffer&& rhs) noexcept;

    CommandBuffer& operator=(const CommandBuffer&) = delete;
    CommandBuffer& operator=(CommandBuffer&& rhs) noexcept;

    void BindPipeline(Pipeline* const pipeline);
    void BindDescriptorSet(DescriptorSet* const set);
    void SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
    void SetScissorRect(const uint32_t left, const uint32_t top, const uint32_t right, const uint32_t bottom);
    void BeginRenderPass(RenderPass* const render_pass, FrameBuffer* const frame_buffer, const std::span<const ClearValueColor> colors, float depth, uint8_t stencil);
    void EndRenderPass();
    void TextureMemoryBarrier(Texture* const texture, const MemoryState before, const MemoryState after);
    void BufferMemoryBarrier(Buffer* const buffer, const MemoryState before, const MemoryState after);
    void Reset();
    void Close();
    void DrawInstanced(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);
    void DrawIndexed(const uint32_t index_count, const uint32_t instance_count, const uint32_t first_index, const uint32_t vertex_offset, const uint32_t first_instance);
    void SetIndexBuffer(BufferView* const buffer_view);
    void SetVertexBuffer(const uint32_t slot, BufferView* const buffer_view);
    void CopyBuffer(Buffer* const dst_buffer, const uint64_t dst_offset, Buffer* const src_buffer, const uint64_t src_offset, const size_t size);
    //void CopyBufferToTexture(Buffer* src_buffer, Texture* dst_texture, )

private:

    Device* device_;

    ComPtr<ID3D12CommandAllocator> allocator_;
    ComPtr<ID3D12GraphicsCommandList4> list_;
};

}