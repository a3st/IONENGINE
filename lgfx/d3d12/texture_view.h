// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "../types.h"
#include "d3d12.h"
#include "descriptor_pool.h"

namespace lgfx {

class TextureView {

public:

    TextureView();
    ~TextureView();
    TextureView(Device* device, DescriptorPool* pool, Texture* texture, const TextureViewDesc& desc);
    TextureView(const TextureView&) = delete;
    TextureView(TextureView&& rhs) noexcept;

    TextureView& operator=(const TextureView&) = delete;
    TextureView& operator=(TextureView&& rhs) noexcept;

    Texture* GetTexture() const;
    const TextureViewDesc& GetViewDesc() const;
    DescriptorPtr GetDescriptorPtr() const;

private:

    DescriptorPool* pool_;
    Texture* texture_;
    DescriptorPtr ptr_;
    TextureViewDesc view_desc_;

    DescriptorPtr CreateRenderTarget(Device* device, DescriptorPool* pool, Texture* texture, const TextureViewDesc& desc);
};

}