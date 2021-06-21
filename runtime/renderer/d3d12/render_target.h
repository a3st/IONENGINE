// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class RenderTarget {
friend class Swapchain;
public:

    RenderTarget() {

    }

private:

    ComPtr<ID3D12Resource> m_resource;

};


}