// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class BindingSet {
public:

    virtual ~BindingSet() = default;

    virtual void write(const WriteBindingSet& write_binding_set) = 0;
};

}