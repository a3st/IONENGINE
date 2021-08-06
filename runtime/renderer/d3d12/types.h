// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class D3DInstance;
class D3DAdapter;
class D3DDevice;
class D3DCommandQueue;
class D3DShader;
class D3DRenderPass;

enum class Format {
    Undefined = DXGI_FORMAT_UNKNOWN,
    Test = DXGI_FORMAT_R8G8B8A8_UNORM
};

}