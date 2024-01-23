// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "renderer/rhi/device.hpp"
#include "descriptor_allocator.hpp"
#include "command_allocator.hpp"
#include "upload_context.hpp"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <dxcapi.h>
#include <winrt/base.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

namespace ionengine {

namespace platform {
class Window;
}

namespace renderer {

namespace rhi {

class DX12Device : public Device {
public:

    DX12Device(platform::Window const& window);

	~DX12Device();

	auto create_allocator(size_t const block_size, size_t const shrink_size, BufferUsageFlags const flags) -> core::ref_ptr<MemoryAllocator> override;

	auto allocate_command_buffer(CommandBufferType const buffer_type) -> core::ref_ptr<CommandBuffer> override;

	virtual auto create_shader(

    ) -> core::ref_ptr<Shader> override;

    auto create_texture(
		uint32_t const width,
        uint32_t const height,
        uint32_t const depth,
        TextureFormat const format,
        TextureDimension const dimension,
        TextureUsageFlags const flags
	) -> Future<Texture> override;

    auto create_buffer(
		MemoryAllocator& allocator, 
		size_t const size, 
		BufferUsageFlags const flags, 
		std::span<uint8_t const> const data
	) -> Future<Buffer> override;

    auto write_buffer(core::ref_ptr<Buffer> buffer, uint64_t const offset, std::span<uint8_t const> const data) -> Future<Buffer> override;

	auto submit_command_lists(std::span<core::ref_ptr<CommandBuffer>> const command_buffers) -> void override;

	auto wait_for_idle() -> void override;

	auto request_next_swapchain_buffer() -> core::ref_ptr<Texture> override;

	auto present() -> void override;

	auto resize_swapchain_buffers(uint32_t const width, uint32_t const height) -> void override;

private:

#ifdef _DEBUG
    winrt::com_ptr<ID3D12Debug> debug;
#endif
    winrt::com_ptr<IDXGIFactory4> factory;
    winrt::com_ptr<IDXGIAdapter1> adapter;
    winrt::com_ptr<ID3D12Device4> device;
	winrt::com_ptr<IDXGISwapChain3> swapchain;

	struct QueueInfo {
		winrt::com_ptr<ID3D12CommandQueue> queue;
		winrt::com_ptr<ID3D12Fence> fence;
		uint64_t fence_value;
	};
	std::vector<QueueInfo> queue_infos;
	HANDLE fence_event;

	core::ref_ptr<PoolDescriptorAllocator> pool_allocator{nullptr};
	core::ref_ptr<UploadContext> upload_context{nullptr};

	struct FrameInfo {
		core::ref_ptr<Texture> swapchain_buffer;
		core::ref_ptr<CommandAllocator> command_allocator;
		uint64_t present_fence_value;
	};
	std::vector<FrameInfo> frame_infos;
	uint32_t frame_index{0};

	std::vector<ID3D12CommandList*> command_batches;
};

}

}

}