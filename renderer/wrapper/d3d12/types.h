// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

struct DescriptorTableDesc {
	D3D12_DESCRIPTOR_HEAP_TYPE type;
	uint32 count;
	uint32 offset;
	bool compute;
};

}