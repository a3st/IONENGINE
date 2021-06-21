// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#ifdef IONENGINE_PLATFORM_WIN32

namespace ionengine::platform {

const std::array<const char*, 2> vk_extensions = { 
    "VK_KHR_surface", 
    "VK_KHR_win32_surface" 
};

}

#endif