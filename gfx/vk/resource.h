// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class VKResource : public Resource {
public:

    VKResource() {

    }

    void bind_memory(Memory* memory, const uint64 offset) override {

    }

    byte* map() override {
        return nullptr;
    }

    void unmap() override {

    }

    ResourceType get_type() const override { return m_type; }
    const ResourceDesc& get_desc() const override { return m_desc; }

private:

    VKMemory* m_memory;

    ResourceType m_type;
    ResourceDesc m_desc;
};

}
