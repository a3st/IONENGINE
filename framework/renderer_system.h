// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "lib/singleton.h"
#include "renderer/base_renderer.h"
#include "renderer/api.h"
#include "platform/window.h"

namespace ionengine::renderer {

class RenderSystem final : public Singleton<RenderSystem> {
DECLARE_SINGLETON(RenderSystem)
public:

    template<typename T = BaseRenderer>
    void create_renderer(platform::Window& window) {
        
    }

protected:

    RenderSystem() : m_adapter(m_instance), m_device(m_adapter) {
        
        std::wcout << 
            "device_id: " << m_adapter.get_device_id() <<
            "\ndevice_name: " << m_adapter.get_device_name() << 
            "\nmemory_device: " << m_adapter.get_video_memory() << 
            "\nmemory_system: " << m_adapter.get_system_memory() << 
        std::endl;
    }

private:

    Instance m_instance;
    Adapter m_adapter;
    Device m_device;
};

}