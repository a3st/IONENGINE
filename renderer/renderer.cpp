// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/renderer.h>
#include <scene/scene.h>
#include <scene/scene_visitor.h>
#include <scene/mesh_node.h>
#include <scene/transform_node.h>
#include <scene/camera_node.h>

using namespace ionengine;
using namespace ionengine::renderer;

class MeshVisitor : public scene::SceneVisitor {
public:

    MeshVisitor(RenderQueue& render_queue) : _render_queue(&render_queue) { }

    void operator()(scene::MeshNode& other) {

        if(other.mesh().is_ok()) {
            for(auto& surface : other.mesh()->surfaces()) {
                
                auto instance = SurfaceInstance {
                    .model = other.transform_global()
                };

                _render_queue->push(surface, instance, other.material(surface->material_index()));
            }
        }
    }

    // Default
    void operator()(scene::TransformNode& other) { }
    void operator()(scene::CameraNode& other) { }

private:

    RenderQueue* _render_queue;
};

Renderer::Renderer(platform::Window& window, asset::AssetManager& asset_manager) : 
    _device(0, backend::SwapchainDesc { .window = &window, .sample_count = 1, .buffer_count = 2 }),
    _asset_manager(&asset_manager) {

    auto [mesh_sender, mesh_receiver] = lib::make_channel<asset::AssetEvent<asset::Mesh>>();
    asset_manager.mesh_pool().event_dispatcher().add(std::move(mesh_sender));
    _mesh_event_receiver.emplace(std::move(mesh_receiver));

    auto [technique_sender, technique_receiver] = lib::make_channel<asset::AssetEvent<asset::Technique>>();
    asset_manager.technique_pool().event_dispatcher().add(std::move(technique_sender));
    _technique_event_receiver.emplace(std::move(technique_receiver));

    _frame_graph.emplace(_device);
    _upload_context.emplace(_device);

    _shader_cache.emplace(_device);
    _geometry_cache.emplace(_device);
    _pipeline_cache.emplace(_device);

    _width = window.client_size().width;
    _height = window.client_size().height;

    _gbuffer_albedo = GPUTexture::render_target(_device, backend::Format::RGBA8, window.client_size().width, window.client_size().height);
}

void Renderer::update(float const delta_time) {

    // Mesh Event Update
    {
        asset::AssetEvent<asset::Mesh> mesh_event;
        while(_mesh_event_receiver.value().try_receive(mesh_event)) {
            auto event_visitor = make_visitor(
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Loaded>& event) {
                    std::cout << std::format("[Debug] Renderer created GeometryBuffers from '{}'", event.asset.path().string()) << std::endl;

                    for(auto& surface : event.asset->surfaces()) {
                        _geometry_cache.value().get(_upload_context.value(), *surface);
                    }
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
                    _shader_cache.value().get(*event.asset);
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

    uint32_t const frame_index = _frame_graph.value().wait();

    scene::CameraNode* camera = scene.graph().find_by_name<scene::CameraNode>("MainCamera");

    _deffered_queue.clear();

    MeshVisitor mesh_visitor(_deffered_queue);
    scene.graph().visit(scene.graph().begin(), scene.graph().end(), mesh_visitor);

    build_frame_graph(_width, _height, frame_index);
    
    _frame_graph.value().execute();
}

void Renderer::resize(uint32_t const width, uint32_t const height) {

}

void Renderer::build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const frame_index) {

    CreateColorInfo gbuffer_albedo_info = {
        .attachment = _gbuffer_albedo,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = lib::math::Color(0.1f, 0.5f, 0.2f, 1.0f)
    };

    _frame_graph.value().add_pass(
        "gbuffer", 
        width,
        height,
        std::span<CreateColorInfo const>(&gbuffer_albedo_info, 1),
        std::nullopt,
        std::nullopt,
        [&](RenderPassContext const& context) {
            
            for(auto const& batch : _deffered_queue) {
                
                auto geometry_buffer = _geometry_cache.value().get(_upload_context.value(), *batch.surface);
                auto pipeline = _pipeline_cache.value().get(_shader_cache.value(), *batch.material, "gbuffer", context.render_pass());

                pipeline->bind(context.command_list());

                for(auto const& instance : batch.instances) {
                    // ShaderUniformBinder = pipeline->allocate_uniform_binder();
                    // std::shared_ptr<GPUBuffer> buffer = pipeline->allocate_cbuffer();
                    // binder.bind_sampler("color", std::shared_ptr<GPUTexture> texture);
                    // binder.bind_cbuffer("material", std::shared_ptr<GPUBuffeR> buffer);
                    //
                }
            }
        }
    );

    CreateColorInfo swapchain_info = {
        .attachment = nullptr,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = lib::math::Color(0.1f, 0.5f, 0.2f, 1.0f)
    };

    _frame_graph.value().add_pass(
        "present",
        width,
        height,
        std::span<CreateColorInfo const>(&swapchain_info, 1),
        std::nullopt,
        std::nullopt,
        RenderPassDefaultFunc
    );

    /*_frame_graph.value().add_pass(
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

                pipeline_target = _context.device().create_pipeline(
                    _shader_prog_2.value().layout(),
                    //_offscreen_quad.value().vertex_declaration(),
                    _shader_prog_2.value().shaders(),
                    backend::RasterizerDesc { backend::FillMode::Solid, backend::CullMode::None },
                    backend::DepthStencilDesc { backend::CompareOp::Always, false },
                    backend::BlendDesc { false, backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add, backend::Blend::One, backend::Blend::Zero, backend::BlendOp::Add },
                    context.render_pass(),
                    backend::InvalidHandle<backend::CachePipeline>()
                );

                _pipelines.insert({ context.render_pass().index(), pipeline_target });
            }

            _context.device().bind_pipeline(context.command_list(), pipeline_target);

            frontend::ShaderUniformBinder binder(_context, _shader_prog_2.value());
            binder.bind_texture(_shader_prog_2.value().get_uniform_by_name("albedo"), context.attachment(0), _sampler);
            binder.update(context.command_list());

            _offscreen_quad.value().bind(context.command_list());
        }
    );*/
}
