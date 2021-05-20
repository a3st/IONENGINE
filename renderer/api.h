// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#define RENDERER_API_D3D12

#ifdef RENDERER_API_D3D12

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

#include "d3d12/utils.h"
#include "d3d12/instance.h"
#include "d3d12/adapter.h"
#include "d3d12/device.h"
#include "d3d12/swapchain.h"

#endif