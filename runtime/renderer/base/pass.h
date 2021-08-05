// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class RenderPass {
public:
    virtual ~RenderPass() = default;
    virtual const RenderPassDesc& get_desc() const = 0;
};

}