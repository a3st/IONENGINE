// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::rendersystem {

#ifdef IONENGINE_PLATFORM_WIN32
constexpr uint32 render_texture_pool_max = 32;
constexpr uint32 vertex_buffer_pool_max = 128;
constexpr uint32 index_buffer_pool_max = 128;
#endif // IONENGINE_PLATFORM_WIN32

}