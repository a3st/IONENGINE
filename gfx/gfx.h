// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "base/types.h"
#include "base/fence.h"
#include "base/memory.h"
#include "base/resource.h"
#include "base/sampler.h"
#include "base/render_pass.h"
#include "base/frame_buffer.h"
#include "base/descriptor_set_layout.h"
#include "base/descriptor_set.h"
#include "base/pipeline.h"
#include "base/command_list.h"
#include "base/device.h"

#ifdef IONENGINE_GFX_D3D12

#include <d3d12.h>
#include <dxgi1_6.h>
#include <winrt/base.h>
#include <dxgidebug.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "windowsapp.lib")

#include "d3d12/types.h"
#include "d3d12/conversion.h"
#include "d3d12/fence.h"
#include "d3d12/memory.h"
#include "d3d12/resource.h"
#include "d3d12/sampler.h"
#include "d3d12/render_pass.h"
#include "d3d12/frame_buffer.h"
#include "d3d12/descriptor_set_layout.h"
#include "d3d12/descriptor_set.h"
#include "d3d12/pipeline.h"
#include "d3d12/command_list.h"
#include "d3d12/device.h"

#endif

#ifdef IONENGINE_GFX_VK

#ifdef IONENGINE_PLATFORM_WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

#ifdef IONENGINE_PLATFORM_ANDROID
#define VK_USE_PLATFORM_ANDROID_KHR
#endif

#include <vulkan/vulkan.hpp>

#pragma comment(lib, "vulkan-1.lib")

#include "vk/types.h"
#include "vk/conversion.h"
#include "vk/fence.h"
#include "vk/memory.h"
#include "vk/resource.h"
#include "vk/sampler.h"
#include "vk/pipeline.h"
#include "vk/command_list.h"
#include "vk/device.h"

#endif