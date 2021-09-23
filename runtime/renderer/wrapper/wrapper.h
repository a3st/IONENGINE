// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "base/types.h"
#include "base/utils.h"
#include "base/memory.h"
#include "base/resource.h"
#include "base/shader.h"
#include "base/descriptors.h"
#include "base/view.h"
#include "base/pass.h"
#include "base/pipeline.h"
#include "base/fence.h"
#include "base/swapchain.h"
#include "base/command_buffer.h"
#include "base/queue.h"
#include "base/device.h"
#include "base/adapter.h"

#ifdef IONENGINE_RENDERER_D3D12

#include <d3d12.h>
#include <dxgi1_6.h>
#include <winrt/base.h>
#include <dxgidebug.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "DXGUID.LIB")
#pragma comment(lib, "windowsapp.lib")

#include "d3d12/types.h"
#include "d3d12/utils.h"
#include "d3d12/memory.h"
#include "d3d12/resource.h"
#include "d3d12/shader.h"
#include "d3d12/descriptors.h"
#include "d3d12/view.h"
#include "d3d12/pass.h"
#include "d3d12/pipeline.h"
#include "d3d12/fence.h"
#include "d3d12/swapchain.h"
#include "d3d12/command_buffer.h"
#include "d3d12/queue.h"
#include "d3d12/device.h"

#endif

#ifdef IONENGINE_RENDERER_VK

#ifdef IONENGINE_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef IONENGINE_PLATFORM_ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR
#endif

#include <vulkan/vulkan.hpp>

#pragma comment(lib, "vulkan-1.lib")

#include "vk/types.h"
#include "vk/utils.h"
#include "vk/swapchain.h"
#include "vk/queue.h"
#include "vk/shader.h"
#include "vk/pipeline.h"
#include "vk/device.h"
#include "vk/adapter.h"
#include "vk/instance.h"

#endif