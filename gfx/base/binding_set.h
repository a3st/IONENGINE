// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

template<class B = backend::base>
class BindingSet {
public:

    void write(const WriteBindingSet<B>& write_binding_set);
};

}