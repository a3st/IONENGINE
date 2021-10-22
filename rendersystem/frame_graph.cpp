// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "frame_graph.h"

using namespace ionengine::rendersystem;

FrameGraphResource::FrameGraphResource() {

}

FrameGraphResource::FrameGraphResource(lgfx::Texture* texture, lgfx::TextureView* texture_view) :
    attachment_{ texture, texture_view }, type_(FrameGraphResourceType::Attachment) {

}

FrameGraphTaskBuilder::FrameGraphTaskBuilder() {

}

FrameGraphResource FrameGraphTaskBuilder::Create(const FrameGraphResourceType type, const FrameGraphExternalResourceInfo& info) {

    FrameGraphResource resource;
    switch(type) {
        case FrameGraphResourceType::Attachment: {
            resource = FrameGraphResource(info.attachment.texture, info.attachment.view);
            break;
        }
        case FrameGraphResourceType::Buffer: {
            break;
        }
    }
    return resource;
}

void FrameGraphTaskBuilder::Read(FrameGraphResource* resource) {

}

void FrameGraphTaskBuilder::Write(FrameGraphResource* resource, const FrameGraphResourceWriteOp write_op, const Color& clear_color) {

}

FrameGraph::FrameGraph() {

}

FrameGraph::FrameGraph(lgfx::Device* device) {

    command_buffer_ = lgfx::CommandBuffer(device, lgfx::CommandBufferType::kGraphics);
}

FrameGraph::FrameGraph(FrameGraph&& rhs) noexcept {

}

FrameGraph& FrameGraph::operator=(FrameGraph&& rhs) noexcept {

    return *this;
}

void FrameGraph::Execute() {

}