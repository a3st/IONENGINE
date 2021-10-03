// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class Pipeline {
public:

    virtual ~Pipeline() = default;

    virtual std::vector<byte> get_pipeline_cache() const = 0;
    
    virtual PipelineType get_type() const = 0;

    virtual BindingSetLayout* get_binding_set_layout() = 0;
};

}