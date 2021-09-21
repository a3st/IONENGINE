// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

class D3DInstance;
class D3DAdapter;
class D3DDevice;
class D3DCommandQueue;
class D3DSwapchain;
class D3DShader;
class D3DPipeline;
class D3DDescriptorSetLayout;
class D3DDescriptorSet;
class D3DDescriptorPool;
class D3DRenderPass;
class D3DResource;
class D3DMemory;
class D3DFence;
class D3DView;

struct DescriptorTableDesc {
	D3D12_DESCRIPTOR_HEAP_TYPE type;
	uint32 count;
	uint32 offset;
	bool compute;
};

}