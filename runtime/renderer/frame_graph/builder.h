// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

class RenderPassBuilder {
friend class FrameGraph;
public:

    RenderPassBuilder() {
        
    }

    FrameGraphResourceHandle add_input(const std::string& name, const AttachmentDesc& desc) {
        return { };
    }

    FrameGraphResourceHandle add_output(const std::string& name, const AttachmentDesc& desc) {
        return { };
    }

protected:



private:

};

}