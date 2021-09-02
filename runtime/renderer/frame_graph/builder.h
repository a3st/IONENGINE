// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine {

class RenderPassBuilder {
friend class FrameGraph;
public:

    RenderPassBuilder() {
        
    }

    FrameGraphResourceHandle add_input(const std::string& name, const RenderPassLoadOp load_op, const ClearValueColor& clear_value) {
        return { };
    }

    FrameGraphResourceHandle add_output(const std::string& name, const RenderPassLoadOp load_op, const ClearValueColor& clear_value) {
        return { };
    }

    FrameGraphResourceHandle add_output(const std::string& name, const RenderPassLoadOp load_op) {
        return { };
    }

protected:



private:

};

}