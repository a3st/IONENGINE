// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class FrameGraphResource {
public:

private:

    uint64 m_id;
};

class FrameGraphResourceManager {
public:

    FrameGraphResourceManager() {

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