// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

/**
    @brief View (backend::base)

    View base template class
*/

template<class B = backend::base>
class View {
public:

    ViewType get_type() const;
};

template<class B = backend::base>
const ViewDesc& get_view_desc(View<B>* view);

}