// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "types.h"

#ifdef LGFX_BACKEND_D3D12

#include "d3d12/device.h"
#include "d3d12/command_buffer.h"
#include "d3d12/descriptor_pool.h"
#include "d3d12/memory_pool.h"
#include "d3d12/fence.h"
#include "d3d12/frame_buffer.h"
#include "d3d12/render_pass.h"
#include "d3d12/texture_view.h"
#include "d3d12/texture.h"
#include "d3d12/buffer_view.h"
#include "d3d12/buffer.h"
#include "d3d12/descriptor_layout.h"
#include "d3d12/pipeline.h"
#include "d3d12/descriptor_set.h"

#endif