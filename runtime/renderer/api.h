// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

//#define RENDERER_API_D3D12

#ifdef RENDERER_API_D3D12

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

#include "d3d12/types.h"
#include "d3d12/utils.h"
#include "d3d12/image_view.h"
#include "d3d12/instance.h"
#include "d3d12/adapter.h"
#include "d3d12/device.h"
#include "d3d12/swapchain.h"
#include "d3d12/descriptor_pool.h"
#include "d3d12/memory_pool.h"

#endif

#ifdef RENDERER_API_VK

#include <vulkan.h>

#pragma comment(lib, "vulkan-1.lib")

#include "vk/..."

#endif