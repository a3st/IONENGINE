// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

template<class B = backend::base>
class Pipeline {
public:

    std::vector<byte> get_pipeline_cache() const;
    
    PipelineType get_type() const;

    BindingSetLayout* get_binding_set_layout();
};

}