// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/fmt.h"

namespace ionengine::renderer {

void throw_if_failed(const HRESULT hr) {
	if (FAILED(hr)) {
		throw std::runtime_error(format<std::string>("An error has occurred {x:8}", hr));
	}
}

}