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
            for(size_t i = 0; i < other.mesh()->surfaces().size(); ++i) {
                
                auto instance = SurfaceInstance {
                    .model = other.transform_global()
                };

                _render_queue->push(other.mesh(), static_cast<uint32_t>(i), instance, other.material(other.mesh()->surfaces()[i].material_index));
            }
        }
    }

    // Default
    void operator()(scene::TransformNode& other) { }
    void operator()(scene::CameraNode& other) { }
    void operator()(scene::PointLightNode& other) { }

private:

    RenderQueue* _render_queue;
};

Renderer::Renderer(platform::Window& window, asset::AssetManager& asset_manager, DefaultAssetDesc const& asset_desc) : 
    _device(0, backend::SwapchainDesc { .window = &window, .sample_count = 1, .buffer_count = 2 }),
    _asset_manager(&asset_manager) {

    auto [mesh_sender, mesh_receiver] = lib::make_channel<asset::AssetEvent<asset::Mesh>>();
    asset_manager.mesh_pool().event_dispatcher().add(std::move(mesh_sender));
    _mesh_event_receiver.emplace(std::move(mesh_receiver));

    auto [technique_sender, technique_receiver] = lib::make_channel<asset::AssetEvent<asset::Technique>>();
    asset_manager.technique_pool().event_dispatcher().add(std::move(technique_sender));
    _technique_event_receiver.emplace(std::move(technique_receiver));

    auto [texture_sender, texture_receiver] = lib::make_channel<asset::AssetEvent<asset::Texture>>();
    asset_manager.texture_pool().event_dispatcher().add(std::move(texture_sender));
    _texture_event_receiver.emplace(std::move(texture_receiver));

    _frame_graph.emplace(_device);
    _upload_context.emplace(_device);

    _shader_cache.emplace(_device);
    _geometry_cache.emplace(_device);
    _pipeline_cache.emplace(_device);
    _texture_cache.emplace(_device);

    _width = window.client_width();
    _height = window.client_height();

    _gbuffer_positions.resize(2);
    _gbuffer_albedos.resize(2);
    _gbuffer_normals.resize(2);
    _gbuffer_roughmetals.resize(2);
    _depth_stencils.resize(2);

    _material_buffer.resize(512);

    for(uint32_t i = 0; i < 2; ++i) {

        _gbuffer_positions.at(i) = GPUTexture::render_target(_device, backend::Format::RGBA16_FLOAT, _width, _height);
        _gbuffer_albedos.at(i) = GPUTexture::render_target(_device, backend::Format::RGBA8, _width, _height);
        _gbuffer_normals.at(i) = GPUTexture::render_target(_device, backend::Format::RGBA16_FLOAT, _width, _height);
        _gbuffer_roughmetals.at(i) = GPUTexture::render_target(_device, backend::Format::RGBA8, _width, _height);
        _depth_stencils.at(i) = GPUTexture::depth_stencil(_device, backend::Format::D32, _width, _height);

        _world_cbuffer_pools.emplace_back(_device, 256, 32);
        _material_cbuffer_pools.emplace_back(_device, 512, 32);
        _object_sbuffer_pools.emplace_back(_device, 32 * static_cast<uint32_t>(sizeof(ObjectSBuffer)), static_cast<uint32_t>(sizeof(ObjectSBuffer)), 32);
    }

    _deffered_technique = asset_desc.deffered;
    _deffered_technique.wait();

    _quad_mesh = asset_desc.quad;
    _quad_mesh.wait();
}

void Renderer::update(float const delta_time) {

    // Mesh Event Update
    {
        asset::AssetEvent<asset::Mesh> mesh_event;
        while(_mesh_event_receiver.value().try_receive(mesh_event)) {
            auto event_visitor = make_visitor(
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Loaded>& event) {
                    std::cout << std::format("[Debug] Renderer created GeometryBuffers from '{}'", event.asset.path().string()) << std::endl;

                    for(size_t i = 0; i < event.asset->surfaces().size(); ++i) {
                        _geometry_cache.value().get(_upload_context.value(), *event.asset, static_cast<uint32_t>(i));
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

    // Texture Event Update
    {
        asset::AssetEvent<asset::Texture> texture_event;
        while(_texture_event_receiver.value().try_receive(texture_event)) {
            auto event_visitor = make_visitor(
                [&](asset::AssetEventData<asset::Texture, asset::AssetEventType::Loaded>& event) {
                    std::cout << std::format("[Debug] Renderer created GPUTexture from '{}'", event.asset.path().string()) << std::endl;
                    _texture_cache.value().get(_upload_context.value(), *event.asset);
                },
                // Default
                [&](asset::AssetEventData<asset::Texture, asset::AssetEventType::Unloaded>& event) { },
                [&](asset::AssetEventData<asset::Texture, asset::AssetEventType::Added>& event) { },
                [&](asset::AssetEventData<asset::Texture, asset::AssetEventType::Removed>& event) { }
            );

            std::visit(event_visitor, texture_event.data);
        }
    }
}

void Renderer::render(scene::Scene& scene) {

    uint32_t const frame_index = _frame_graph.value().wait();
    scene::CameraNode* camera = scene.graph().find_by_name<scene::CameraNode>("MainCamera");

    _deffered_queue.clear();

    _world_cbuffer_pools.at(frame_index).reset();
    _material_cbuffer_pools.at(frame_index).reset();
    _object_sbuffer_pools.at(frame_index).reset();

    MeshVisitor mesh_visitor(_deffered_queue);
    scene.graph().visit(scene.graph().begin(), scene.graph().end(), mesh_visitor);

    camera->calculate_matrices();

    build_frame_graph(_width, _height, frame_index, camera);
    
    _frame_graph.value().execute();
}

void Renderer::resize(uint32_t const width, uint32_t const height) {

}

void Renderer::build_frame_graph(uint32_t const width, uint32_t const height, uint32_t const frame_index, scene::CameraNode* camera) {

    auto depth_stencil_info = CreateDepthStencilInfo {
        .attachment = _depth_stencils.at(frame_index),
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_depth = 1.0f,
        .clear_stencil = 0x0
    };

    auto gbuffer_infos = std::array<CreateColorInfo, 4> {
        CreateColorInfo {
            .attachment = _gbuffer_positions.at(frame_index),
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.0f, 0.0f, 0.0f, 1.0f)
        },
        CreateColorInfo {
            .attachment = _gbuffer_albedos.at(frame_index),
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
        },
        CreateColorInfo {
            .attachment = _gbuffer_normals.at(frame_index),
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.0f, 0.0f, 0.0f, 1.0f)
        },
        CreateColorInfo {
            .attachment = _gbuffer_roughmetals.at(frame_index),
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.0f, 0.0f, 0.0f, 1.0f)
        }
    };

    auto world = WorldCBuffer {
        .viewproj = camera->transform_view() * camera->transform_projection()
    };

    std::shared_ptr<GPUBuffer> world_buffer = _world_cbuffer_pools.at(frame_index).allocate();
    world_buffer->copy_data(_upload_context.value(), std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&world), sizeof(WorldCBuffer)));

    _frame_graph.value().add_pass(
        "gbuffer", 
        width,
        height,
        gbuffer_infos,
        std::nullopt,
        depth_stencil_info,
        [=](RenderPassContext const& context) {
            
            for(auto const& batch : _deffered_queue) {

                asset::MaterialPass const& material_pass = batch.material->passes()[0];
                _material_samplers.clear();

                auto [pipeline, shader_program] = _pipeline_cache.value().get(_shader_cache.value(), *material_pass.technique, material_pass.parameters, context.render_pass());
                pipeline->bind(context.command_list());

                auto geometry_buffer = _geometry_cache.value().get(_upload_context.value(), *batch.mesh, batch.surface_index);
                geometry_buffer->bind(context.command_list());

                ShaderUniformBinder binder(_device, *shader_program);

                apply_material_parameters(binder, *batch.material, frame_index);

                for(auto& sampler : _material_samplers) {
                    if(sampler->memory_state() != backend::MemoryState::ShaderRead) {
                        sampler->barrier(context.command_list(), backend::MemoryState::ShaderRead);
                    }
                }

                std::shared_ptr<GPUBuffer> objects_buffer = _object_sbuffer_pools.at(frame_index).allocate();

                uint32_t const world_location = shader_program->location_by_uniform_name("world");
                binder.bind_cbuffer(world_location, *world_buffer);

                std::vector<ObjectSBuffer> objects;

                for(auto const& instance : batch.instances) {
                    objects.emplace_back(instance.model);
                }

                objects_buffer->copy_data(_upload_context.value(), std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(objects.data()), objects.size() * sizeof(ObjectSBuffer)));

                uint32_t const object_location = shader_program->location_by_uniform_name("object");
                binder.bind_cbuffer(object_location, *objects_buffer);

                binder.update(context.command_list());

                geometry_buffer->draw(context.command_list(), static_cast<uint32_t>(batch.instances.size()));

                for(auto& sampler : _material_samplers) {
                    if(sampler->memory_state() != backend::MemoryState::Common) {
                        sampler->barrier(context.command_list(), backend::MemoryState::Common);
                    }
                }
            }
        }
    );

    CreateColorInfo swapchain_info = {
        .attachment = nullptr,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
    };

    auto gbuffer_input_infos = std::array<CreateInputInfo, 4> {
        CreateInputInfo { .attachment = _gbuffer_positions.at(frame_index) },
        CreateInputInfo { .attachment = _gbuffer_albedos.at(frame_index) },
        CreateInputInfo { .attachment = _gbuffer_normals.at(frame_index) },
        CreateInputInfo { .attachment = _gbuffer_roughmetals.at(frame_index) }
    };

    _frame_graph.value().add_pass(
        "deffered",
        width,
        height,
        std::span<CreateColorInfo const>(&swapchain_info, 1),
        gbuffer_input_infos,
        std::nullopt,
        [=](RenderPassContext const& context) {

            auto parameters = asset::MaterialPassParameters {
                .fill_mode = asset::MaterialPassFillMode::Solid,
                .cull_mode = asset::MaterialPassCullMode::None,
                .depth_stencil = false
            };

            auto geometry = _geometry_cache.value().get(_upload_context.value(), *_quad_mesh, 0);
            auto [pipeline, shader_program] = _pipeline_cache.value().get(_shader_cache.value(), *_deffered_technique, parameters, context.render_pass());

            pipeline->bind(context.command_list());

            ShaderUniformBinder binder(_device, *shader_program);

            uint32_t const albedo_index = shader_program->location_by_uniform_name("albedo");
            binder.bind_texture(albedo_index, *context.input(1));
            binder.update(context.command_list());

            geometry->bind(context.command_list());

            geometry->draw(context.command_list(), 1);
        }
    );
}

void Renderer::apply_material_parameters(ShaderUniformBinder& binder, asset::Material& material, uint32_t const frame_index) {

    for(auto& [parameter_name, parameter] : material.parameters()) {

        if(parameter.is_sampler2D()) {

            auto it = binder.shader_program().uniforms().find(parameter_name);
            if(it == binder.shader_program().uniforms().end()) {
                break;
            }

            if(parameter.as_sampler2D().asset.is_ok()) {

                auto gpu_texture = _texture_cache.value().get(_upload_context.value(), *parameter.as_sampler2D().asset);
                _material_samplers.emplace_back(gpu_texture);

                auto uniform_visitor = make_visitor(
                    [&](ShaderUniformData<ShaderUniformType::Sampler2D> const& data) {
                        binder.bind_texture(data.index, *gpu_texture);
                    },
                    // Default
                    [&](ShaderUniformData<ShaderUniformType::CBuffer> const& data) { },
                    [&](ShaderUniformData<ShaderUniformType::SBuffer> const& data) { },
                    [&](ShaderUniformData<ShaderUniformType::RWBuffer> const& data) { }
                );

                std::visit(uniform_visitor, it->second.data);
            }

        } else {
                        
            auto it = binder.shader_program().uniforms().find("material");
            if(it == binder.shader_program().uniforms().end()) {
                break;
            }

            auto uniform_visitor = make_visitor(
                [&](ShaderUniformData<ShaderUniformType::CBuffer> const& uniform_data) {
                                
                    auto parameter_visitor = make_visitor(
                        [&](asset::MaterialParameterData<asset::MaterialParameterType::F32> const& parameter_data) {
                            std::memcpy(_material_buffer.data() + uniform_data.offsets.at(parameter_name), &parameter_data.value, sizeof(float));
                        },
                        [&](asset::MaterialParameterData<asset::MaterialParameterType::F32x2> const& parameter_data) {
                            std::memcpy(_material_buffer.data() + uniform_data.offsets.at(parameter_name), parameter_data.value.data(), sizeof(lib::math::Vector2f));
                        },
                        [&](asset::MaterialParameterData<asset::MaterialParameterType::F32x3> const& parameter_data) {
                            std::memcpy(_material_buffer.data() + uniform_data.offsets.at(parameter_name), parameter_data.value.data(), sizeof(lib::math::Vector3f));
                        },
                        [&](asset::MaterialParameterData<asset::MaterialParameterType::F32x4> const& parameter_data) {
                            std::memcpy(_material_buffer.data() + uniform_data.offsets.at(parameter_name), parameter_data.value.data(), sizeof(lib::math::Vector4f));
                        },
                        // Default
                        [&](asset::MaterialParameterData<asset::MaterialParameterType::Sampler2D> const& parameter_data) { }
                    );

                    std::visit(parameter_visitor, parameter.data);
                },
                // Default
                [&](ShaderUniformData<ShaderUniformType::Sampler2D> const& uniform_data) { },
                [&](ShaderUniformData<ShaderUniformType::SBuffer> const& data) { },
                [&](ShaderUniformData<ShaderUniformType::RWBuffer> const& data) { }
            );

            std::visit(uniform_visitor, it->second.data);
        }
    }

    std::shared_ptr<GPUBuffer> buffer = _material_cbuffer_pools.at(frame_index).allocate();
    buffer->copy_data(_upload_context.value(), std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(_material_buffer.data()), _material_buffer.size()));

    //uint32_t const material_location = binder.shader_program().location_by_uniform_name("material");
    //binder.bind_cbuffer(material_location, *buffer);
}
