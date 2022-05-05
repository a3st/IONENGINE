// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/renderer.h>
#include <scene/scene.h>
#include <scene/scene_visitor.h>
#include <scene/mesh_node.h>
#include <scene/transform_node.h>
#include <scene/camera_node.h>
#include <asset/asset_manager.h>

using namespace ionengine;
using namespace ionengine::renderer;

class GeometryVisitor : public scene::SceneVisitor {
public:

    GeometryVisitor(frontend::Context& context, std::optional<frontend::GeometryBuffer>& geom_buffer, backend::Handle<backend::Buffer>& buffer) {
        _context = &context;
        geom = &geom_buffer;
        _buffer = &buffer;
    }

    void operator()(scene::MeshNode& other) {

        if(!geom->has_value()) {
            //geom->emplace(*_context, other.surface(), frontend::BufferUsage::Static);
        }

        lib::math::Matrixf matrix = other.transform_global();
            
            _context->device().map_buffer_data(
                *_buffer,
                0,
                std::span<uint8_t const>(
                    reinterpret_cast<uint8_t const*>(matrix.data()),
                    matrix.size() * sizeof(float)
                )
            );
    }

    void operator()(scene::TransformNode& other) {

    }

    void operator()(scene::CameraNode& other) {

        other.calculate_matrices();

        lib::math::Matrixf matrix = other.transform_view();

        _context->device().map_buffer_data(
            *_buffer,
            sizeof(lib::math::Matrixf),
            std::span<uint8_t const>(
                reinterpret_cast<uint8_t const*>(matrix.data()),
                matrix.size() * sizeof(float)
            )
        );

        matrix = other.transform_projection();

        _context->device().map_buffer_data(
            *_buffer,
            sizeof(lib::math::Matrixf) * 2,
            std::span<uint8_t const>(
                reinterpret_cast<uint8_t const*>(matrix.data()),
                matrix.size() * sizeof(float)
            )
        );
    }

private:

    frontend::Context* _context;
    std::optional<frontend::GeometryBuffer>* geom;
    backend::Handle<backend::Buffer>* _buffer;
};

Renderer::Renderer(platform::Window& window, asset::AssetManager& asset_manager) : 
    _context(window, 2),
    _frame_graph(_context),
    _asset_manager(&asset_manager) {

    auto [mesh_sender, mesh_receiver] = lib::make_channel<asset::AssetEvent<asset::Mesh>>();
    asset_manager.mesh_pool().event_dispatcher().add(std::move(mesh_sender));
    _mesh_event_receiver.emplace(std::move(mesh_receiver));

    auto [technique_sender, technique_receiver] = lib::make_channel<asset::AssetEvent<asset::Technique>>();
    asset_manager.technique_pool().event_dispatcher().add(std::move(technique_sender));
    _technique_event_receiver.emplace(std::move(technique_receiver));


    build_frame_graph(window.client_size().width, window.client_size().height, 2);


    _model_buffer.resize(2);

    _model_buffer[0] = _context.device().create_buffer(256, backend::BufferFlags::HostWrite | backend::BufferFlags::ConstantBuffer);
    _model_buffer[1] = _context.device().create_buffer(256, backend::BufferFlags::HostWrite | backend::BufferFlags::ConstantBuffer);

    vertex_declaration = { 
        backend::VertexInputDesc { "POSITION", 0, backend::Format::RGB32, 0, 0 },
        backend::VertexInputDesc { "COLOR", 0, backend::Format::RGB32, 0, 0 },
    };

    _sampler = _context.device().create_sampler(
        backend::Filter::MinMagMipLinear, 
        backend::AddressMode::Wrap, 
        backend::AddressMode::Wrap, 
        backend::AddressMode::Wrap,
        8,
        backend::CompareOp::Always
    );

    //auto quad = frontend::GeometryBuffer::quad(_context);
    //_offscreen_quad.emplace(std::move(quad));
}

void Renderer::update(float const delta_time) {

    // Mesh Event Update
    {
        asset::AssetEvent<asset::Mesh> mesh_event;
        while(_mesh_event_receiver.value().try_receive(mesh_event)) {
            auto event_visitor = make_visitor(
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Loaded>& event) {
                    
                },
                // Default
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Unloaded>& event) { },
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Added>& event) { },
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Removed>& event) { }
            );

            std::visit(event_visitor, mesh_event.data);
        }
    }

    // Technique Event Update
    {
        asset::AssetEvent<asset::Technique> technique_event;
        while(_technique_event_receiver.value().try_receive(technique_event)) {
            auto event_visitor = make_visitor(
                [&](asset::AssetEventData<asset::Technique, asset::AssetEventType::Loaded>& event) {
                    std::cout << std::format("[Debug] Renderer created ShaderProgram from '{}'", event.asset.path().string()) << std::endl;
                    _shader_cache.get(_context, *event.asset);
                },
                // Default
                [&](asset::AssetEventData<asset::Technique, asset::AssetEventType::Unloaded>& event) { },
                [&](asset::AssetEventData<asset::Technique, asset::AssetEventType::Added>& event) { },
                [&](asset::AssetEventData<asset::Technique, asset::AssetEventType::Removed>& event) { }
            );

            std::visit(event_visitor, technique_event.data);
        }
    }
}

void Renderer::render(scene::Scene& scene) {

    auto frame_texture = _context.get_or_wait_previous_frame();

    GeometryVisitor geometry_visitor(_context, _geom_triangle, _model_buffer[_context.frame_index()]);
    scene.graph().visit(scene.graph().begin(), scene.graph().end(), geometry_visitor);

    //_context.submit_or_skip_upload_data();

    _frame_graph.bind_attachment(*_swapchain_buffer, frame_texture);
    _frame_graph.execute();

    _context.swap_buffers();
}

void Renderer::resize(uint32_t const width, uint32_t const height) {

}

void Renderer::build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const buffered_frame_count) {

    _gbuffer_color_buffer = &_frame_graph.add_attachment(
        frontend::AttachmentDesc {
            .name = "gbuffer_color",
            .format = backend::Format::RGBA8,
            .width = width,
            .height = height,
            .flags = backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource
        }
    );

    frontend::CreateColorInfo gbuffer_color_info {
        .attachment = _gbuffer_color_buffer,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = lib::math::Color(0.1f, 0.5f, 0.2f, 1.0f)
    };

    _frame_graph.add_pass(
        frontend::RenderPassDesc {
            .name = "gbuffer",
            .width = width,
            .height = height,
            .color_infos = std::span<frontend::CreateColorInfo const>(&gbuffer_color_info, 1)
        },
        [&](frontend::RenderPassContext const& context) {
            
            backend::Handle<backend::Pipeline> pipeline_target;

            auto it = _pipelines.find(context.render_pass().index());
            if(it != _pipelines.end()) {
                
                pipeline_target = it->second;

            } else {

                /*pipeline_target = _context.device().create_pipeline(
                    _shader_prog.value().layout(),
                    vertex_declaration,
                    _shader_prog.value().shaders(),
                    backend::RasterizerDesc { backend::FillMode::Solid, backend::CullMode::None },
                    backend::DepthStencilDesc { backend::CompareOp::Always, false },
                    backend::BlendDesc { false, backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add, backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add },
                    context.render_pass(),
                    backend::InvalidHandle<backend::CachePipeline>()
                );

                _pipelines.insert({ context.render_pass().index(), pipeline_target });*/
            }

            /*_context.device().bind_pipeline(context.command_list(), pipeline_target);

            frontend::ShaderUniformBinder binder(_context, _shader_prog.value());
            binder.bind_cbuffer(_shader_prog.value().get_uniform_by_name("world"), _model_buffer[_context.frame_index()]);
            binder.update(context.command_list());

            _geom_triangle.value().bind(context.command_list());*/
        }
    );

    _swapchain_buffer = &_frame_graph.add_attachment("swapchain_buffer", backend::MemoryState::Present, backend::MemoryState::Present);

    std::array<frontend::CreateColorInfo, 1> present_colors;

    present_colors[0] = frontend::CreateColorInfo {
        .attachment = _swapchain_buffer,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = lib::math::Color(0.0f, 0.3f, 0.8f, 1.0f)
    };

    std::array<frontend::Attachment*, 1> present_inputs;

    present_inputs[0] = {
        _gbuffer_color_buffer
    };

    _frame_graph.add_pass(
        frontend::RenderPassDesc {
            .name = "present_only_pass",
            .width = width,
            .height = height,
            .color_infos = present_colors,
            .inputs = present_inputs
        },
        [&](frontend::RenderPassContext const& context) {

            backend::Handle<backend::Pipeline> pipeline_target;

            auto it = _pipelines.find(context.render_pass().index());
            if(it != _pipelines.end()) {
                
                pipeline_target = it->second;

            } else {

                /*pipeline_target = _context.device().create_pipeline(
                    _shader_prog_2.value().layout(),
                    //_offscreen_quad.value().vertex_declaration(),
                    _shader_prog_2.value().shaders(),
                    backend::RasterizerDesc { backend::FillMode::Solid, backend::CullMode::None },
                    backend::DepthStencilDesc { backend::CompareOp::Always, false },
                    backend::BlendDesc { false, backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add, backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add },
                    context.render_pass(),
                    backend::InvalidHandle<backend::CachePipeline>()
                );

                _pipelines.insert({ context.render_pass().index(), pipeline_target });*/
            }

            /*_context.device().bind_pipeline(context.command_list(), pipeline_target);

            frontend::ShaderUniformBinder binder(_context, _shader_prog_2.value());
            binder.bind_texture(_shader_prog_2.value().get_uniform_by_name("albedo"), context.attachment(0), _sampler);
            binder.update(context.command_list());

            _offscreen_quad.value().bind(context.command_list());*/
        }
    );

    _frame_graph.build(buffered_frame_count);
}
