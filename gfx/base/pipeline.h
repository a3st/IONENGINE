// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class Pipeline {
public:

    virtual ~Pipeline() = default;
    virtual PipelineType get_type() const = 0;
};

}