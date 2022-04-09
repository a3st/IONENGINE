// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/mesh_renderer.h>

using namespace ionengine::renderer;

MeshRenderer::MeshRenderer(Context& context) {

    Attachment& color_buffer = context.frame_graph().add_attachment(
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

    context.frame_graph().add_pass(
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