// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

template<class B = backend::base>
class View {
public:

    ViewType get_type() const;

    const ViewDesc& get_desc() const;
};

}