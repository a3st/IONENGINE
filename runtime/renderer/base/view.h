// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class View {
public:
    virtual ~View() = default;

    virtual Resource& get_resource() const = 0;
};

}