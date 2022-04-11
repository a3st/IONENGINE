// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/context.h>

using namespace ionengine::renderer;

Context::Context(platform::Window& window, uint32_t const buffered_frame_count) 
    : _device(0, backend::SwapchainDesc { .window = &window, .sample_count = 1, .buffer_count = buffered_frame_count }) {

    
}

void Context::render() {


}

FrameGraph& Context::frame_graph() {

    return _frame_graph;
}

void Context::build_frame_graph() {

    Attachment& color_buffer = _frame_graph.add_attachment(
        AttachmentDesc {
            .name = "color_buffer",
            .format = backend::Format::RGBA8,
            .width = 800,
            .height = 600,
            .flags = backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource
        }
    );

    std::vector<CreateColorInfo> color_infos = {
        {
            .attachment = &color_buffer,
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = Color(0.4f, 0.2f, 0.1f, 1.0f)
        }
    };

    _frame_graph.add_pass(
        RenderPassDesc {
            .name = "gbuffer_only_color",
            .width = 800,
            .height = 600,
            .color_infos = color_infos
        },
        [&, this](RenderPassContext const& context) {

            
        }
    );
}