// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

enum class ImageFormat {
    R8G8B8A8_UNORM = VK_FORMAT_R8G8B8A8_UNORM,
    R8G8B8A8_SRGB = VK_FORMAT_R8G8B8A8_SRGB
};

enum class DesctiptorType {
    Sampler,
    RenderTarget,
    DepthStencil,
    Buffer
};

template<DesctiptorType T>
class DescriptorPool {
};

}