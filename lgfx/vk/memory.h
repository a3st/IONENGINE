// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class VKMemory : public Memory {
public:

    VKMemory() {

    }

    MemoryType get_type() const override { return m_type; }
    ResourceFlags get_flags() const override { return m_flags; }

private:

    MemoryType m_type;
    ResourceFlags m_flags;
};

}
