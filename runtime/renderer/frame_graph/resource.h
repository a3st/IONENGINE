// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

struct AttachmentDesc {
    RenderPassLoadOp load_op;
    RenderPassStoreOp store_op;
    ClearValueColor clear_value;

    bool operator<(const AttachmentDesc& rhs) const {
        return std::tie(load_op, store_op, clear_value) < std::tie(rhs.load_op, rhs.store_op, rhs.clear_value);
    }
};

class FrameGraphResource {
public:

    FrameGraphResource() {

    }

    void bind_view(View& view) {
        m_view = view;
    }

private:

    uint64 m_id;

    std::optional<std::reference_wrapper<View>> m_view;
};

class FrameGraphResourceManager {
public:

    FrameGraphResourceManager() {

    }

    FrameGraphResourceHandle create() {
        
    }

private:


};

class FrameGraphResourceHandle {
friend class FrameGraphResourceManager;
public:

    FrameGraphResourceHandle() {

    }

    FrameGraphResourceHandle(const uint64 id) : m_id(id) {

    }

protected:

    uint64 get_id() const { return m_id; }

private:

    uint64 m_id;
};

}