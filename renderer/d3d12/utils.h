// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/fmt.h"

namespace ionengine::renderer {

void throw_if_failed(const HRESULT hr) {
	if (FAILED(hr)) {
		throw std::runtime_error(format<std::string>("An error has occurred {x:8}", hr));
	}
}

struct UTILS_COMMAND_QUEUE_DESC : public D3D12_COMMAND_QUEUE_DESC {

	static D3D12_COMMAND_QUEUE_DESC as_direct_queue(const int32 priority, const D3D12_COMMAND_QUEUE_FLAGS flags) {
		return { D3D12_COMMAND_LIST_TYPE_DIRECT, priority, flags, 0 };
	}

	static D3D12_COMMAND_QUEUE_DESC as_copy_queue(const int32 priority, const D3D12_COMMAND_QUEUE_FLAGS flags) {
		return { D3D12_COMMAND_LIST_TYPE_COPY, priority, flags, 0 };
	}

	static D3D12_COMMAND_QUEUE_DESC as_compute_queue(const int32 priority, const D3D12_COMMAND_QUEUE_FLAGS flags) {
		return { D3D12_COMMAND_LIST_TYPE_DIRECT, priority, flags, 0 };
	}
};

}