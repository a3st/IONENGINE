// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class CommandList {
public:

    virtual ~CommandList() = default;
    
    virtual void bind_pipeline(Pipeline& pipeline) = 0;
    virtual void close() = 0;
    virtual void reset() = 0;
};

}