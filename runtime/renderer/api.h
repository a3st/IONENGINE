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
#include "d3d12/render_target.h"
#include "d3d12/instance.h"
#include "d3d12/adapter.h"
#include "d3d12/device.h"
#include "d3d12/swapchain.h"
//#include "d3d12/descriptor_pool.h"
//#include "d3d12/memory_pool.h"

#endif

#ifdef RENDERER_API_VK

#ifdef IONENGINE_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef IONENGINE_PLATFORM_ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR
#endif

#include <vulkan/vulkan.h>

#pragma comment(lib, "vulkan-1.lib")

#include "vk/utils.h"
#include "vk/types.h"
#include "vk/instance.h"
#include "vk/adapter.h"
#include "vk/device.h"
#include "vk/swapchain.h"

#endif