// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend.h>
#include <renderer/color.h>

namespace ionengine::renderer {

class FrameGraph {
public:

    FrameGraph() = default;

    FrameGraph& render_pass();
    FrameGraph& build(Backend& backend);

    void reset(Backend& backend);
    void execute(Backend& backend);

private:

};

}