// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class ImageView {
friend class Swapchain;
public:

    ImageView() {

    }

private:

    ComPtr<ID3D12Resource> m_resource;

};


}