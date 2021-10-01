// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class View {
public:
    virtual ~View() = default;

    virtual ViewType get_type() const = 0;

    virtual const ViewDesc& get_desc() const = 0;

    virtual Resource* get_resource() const = 0;

    virtual Sampler* get_sampler() const = 0;
};

}