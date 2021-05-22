// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class Device final {
public:

    Device() {

    }

private:

    ComPtr<ID3D12Device4> m_device;
};

}