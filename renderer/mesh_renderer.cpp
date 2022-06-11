// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/mesh_renderer.h>
#include <renderer/shader_binder.h>
#include <scene/scene.h>
#include <scene/camera_node.h>
#include <scene/mesh_node.h>
#include <scene/point_light_node.h>
#include <scene/scene_visitor.h>
#include <range/v3/view/chunk.hpp>

using namespace ionengine;
using namespace ionengine::renderer;

namespace ionengine::renderer {

class MeshVisitor : public scene::SceneVisitor {
public:

    MeshVisitor(RenderQueue& opaque_queue, RenderQueue& transculent_queue, std::vector<PointLightData>& point_lights) : 
        _opaque_queue(&opaque_queue), 
        _transculent_queue(&transculent_queue), 
        _point_lights(&point_lights) { 
        
    }

    void operator()(scene::MeshNode& other) {

        if(other.mesh()->is_ok()) {
            for(size_t i = 0; i < other.mesh()->as_const_ok().surfaces().size(); ++i) {

                lib::math::Matrixf inverse_model = other.model_global();
                inverse_model.inverse();
                
                auto instance = SurfaceInstance {
                    .model = other.model_global(),
                    .inverse_model = inverse_model
                };

                if(other.material(other.mesh()->as_const_ok().surfaces()[i].material_index)->as_const_ok().blend_mode == asset::MaterialBlendMode::Opaque) {
                    _opaque_queue->push(other.mesh(), static_cast<uint32_t>(i), instance, other.material(other.mesh()->as_const_ok().surfaces()[i].material_index));
                } else {
                    _transculent_queue->push(other.mesh(), static_cast<uint32_t>(i), instance, other.material(other.mesh()->as_const_ok().surfaces()[i].material_index));
                }
            }
        }
    }

    void operator()(scene::PointLightNode& other) { 

        auto global_position = lib::math::Vector3f(
            other.model_global().m30,
            other.model_global().m31,
            other.model_global().m32
        );

        auto point_light = PointLightData {
            .position = global_position,
            .range = other.range(),
            .color = lib::math::Vector3f(other.color().r, other.color().g, other.color().b)
        };

        _point_lights->emplace_back(std::move(point_light));
    }

    // Default
    void operator()(scene::TransformNode& other) { }
    void operator()(scene::CameraNode& other) { }

private:

    RenderQueue* _opaque_queue;
    RenderQueue* _transculent_queue;
    std::vector<PointLightData>* _point_lights;
};

}

MeshRenderer::MeshRenderer(backend::Device& device, UploadManager& upload_manager, std::vector<RTTextureCache>& rt_texture_caches, platform::Window& window, asset::AssetManager& asset_manager) :
    _device(&device),
    _asset_manager(&asset_manager),
    _upload_manager(&upload_manager),
    _rt_texture_caches(&rt_texture_caches),
    _texture_cache(device),
    _geometry_cache(device),
    _width(window.client_width()),
    _height(window.client_height()) {

    for(uint32_t i = 0; i < backend::BACKEND_BACK_BUFFER_COUNT; ++i) {
        _object_pools.emplace_back(*_device, 32, 256, BufferPoolUsage::Dynamic);
        _world_pools.emplace_back(*_device, 4, BufferPoolUsage::Dynamic);
        _point_light_pools.emplace_back(*_device, 512, 2, BufferPoolUsage::Dynamic);

        auto gbuffer = GBufferData {
            .positions = make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA16_FLOAT, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()),
            .albedo = make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()),
            .normals = make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA16_FLOAT, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()),
            .roughness_metalness_ao = make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok())
        };

        _gbuffers.emplace_back(std::move(gbuffer));
        _depth_stencils.emplace_back(make_resource_ptr(GPUTexture::create(*_device, backend::Format::D32_FLOAT, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::DepthStencil).as_ok()));
        _final_images.emplace_back(make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()));
    }

    auto quad_surface_data = asset::SurfaceData::make_quad();
    _quad = make_resource_ptr(GeometryBuffer::load_from_surface(*_device, quad_surface_data).as_ok());
    _upload_manager->upload_geometry_data(_quad, quad_surface_data.vertices.to_span(), quad_surface_data.indices.to_span());
}

MeshRenderer::~MeshRenderer() {

}

void MeshRenderer::update(float const delta_time) {
    
}

void MeshRenderer::resize(uint32_t const width, uint32_t const height) {

    _width = width;
    _height = height;

    _gbuffers.clear();
    _depth_stencils.clear();

    for(uint32_t i = 0; i < backend::BACKEND_BACK_BUFFER_COUNT; ++i) {
        auto gbuffer = GBufferData {
            .positions = make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA16_FLOAT, width, height, 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()),
            .albedo = make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, width, height, 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()),
            .normals = make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA16_FLOAT, width, height, 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()),
            .roughness_metalness_ao = make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, width, height, 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok())
        };

        _gbuffers.emplace_back(std::move(gbuffer));
        _depth_stencils.emplace_back(make_resource_ptr(GPUTexture::create(*_device, backend::Format::D32_FLOAT, width, height, 1, 1, backend::TextureFlags::DepthStencil).as_ok()));
        _final_images.emplace_back(make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, width, height, 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()));
    }
}

void MeshRenderer::render(PipelineCache& pipeline_cache, ShaderCache& shader_cache, NullData& null, FrameGraph& frame_graph, scene::Scene& scene, uint32_t const frame_index) {

    _object_pools.at(frame_index).reset();
    _world_pools.at(frame_index).reset();
    _point_light_pools.at(frame_index).reset();

    _opaque_queue.clear();
    _transculent_queue.clear();
    _point_lights.clear();

    MeshVisitor mesh_visitor(_opaque_queue, _transculent_queue, _point_lights);
    scene.visit_culling_nodes(mesh_visitor);

    _render_camera = scene.graph().find_by_name<scene::CameraNode>("main_camera");

    _render_camera->calculate_matrices();
    _opaque_queue.sort();
    _transculent_queue.sort();

    auto world_buffer = WorldData {
        .view = _render_camera->transform_view(),
        .projection = _render_camera->transform_projection(),
        .camera_position = _render_camera->position(),
        .point_light_count = static_cast<uint32_t>(_point_lights.size()),
        .direction_light_count = 0,
        .spot_light_count = 0
    };

    ResourcePtr<GPUBuffer> world_cbuffer = _world_pools.at(frame_index).allocate();
    _upload_manager->upload_buffer_data(world_cbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&world_buffer), sizeof(WorldData)));

    ResourcePtr<GPUBuffer> point_light_sbuffer = _point_light_pools.at(frame_index).allocate();
    _upload_manager->upload_buffer_data(point_light_sbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(_point_lights.data()), _point_lights.size() * sizeof(PointLightData)));

    auto depth_stencil_info = CreateDepthStencilInfo {
        .attachment = _depth_stencils.at(frame_index),
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_depth = 1.0f,
        .clear_stencil = 0x0
    };

    auto gbuffer_color_infos = std::array<CreateColorInfo, 4> {
        CreateColorInfo {
            .attachment = _gbuffers.at(frame_index).positions,
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.0f, 0.0f, 0.0f, 1.0f)
        },
        CreateColorInfo {
            .attachment = _gbuffers.at(frame_index).albedo,
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
        },
        CreateColorInfo {
            .attachment = _gbuffers.at(frame_index).normals,
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.0f, 0.0f, 0.0f, 1.0f)
        },
        CreateColorInfo {
            .attachment = _gbuffers.at(frame_index).roughness_metalness_ao,
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.0f, 0.0f, 0.0f, 1.0f)
        }
    };

    frame_graph.add_pass(
        "gbuffer",
        _width,
        _height,
        gbuffer_color_infos,
        std::nullopt,
        depth_stencil_info,
        [&, frame_index, world_cbuffer](RenderPassContext& context) -> TaskCreateInfo {
            
            uint16_t const thread_count = 16;
            auto chunks = _opaque_queue | ranges::views::chunk(thread_count);

            std::vector<ResourcePtr<CommandList>> command_lists(chunks.size());

            for(size_t i = 0; i < chunks.size(); ++i) {

                ResourcePtr<CommandList> command_list = context.command_pool->allocate();
                command_lists.at(i) = command_list;

                lib::thread_pool().push(
                    lib::TaskPriorityFlags::High,
                    [&, command_list, i, chunks]() {

                        backend::Handle<backend::Pipeline> current_pipeline = backend::InvalidHandle<backend::Pipeline>();

                        for(auto const& batch : chunks.at(i)) {

                            ResourcePtr<Shader> shader = shader_cache.get(batch.material, "gbuffer");

                            pipeline = pipeline_cache.get(*shader, context.render_pass->render_pass);
                            
                            if(current_pipeline != pipeline) {
                                _device->bind_pipeline(command_list->as_const_ok().resource.command_list, pipeline);
                                current_pipeline = pipeline;
                            }

                            ShaderBinder binder(*shader, null);

                            apply_material(binder, *shader, *batch.material, command_list->as_const_ok().resource.command_list, frame_index);

                            std::vector<ObjectData> object_buffers;
                            for(auto const& instance : batch.instances) {
                                object_buffers.emplace_back(instance.model, instance.inverse_model);
                            }

                            ResourcePtr<GPUBuffer> object_sbuffer = _object_pools.at(frame_index).allocate();
                            _upload_manager->upload_buffer_data(object_sbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(object_buffers.data()), object_buffers.size() * sizeof(ObjectData)));

                            uint32_t const world_location = shader->location_uniform_by_name("world");
                            uint32_t const object_location = shader->location_uniform_by_name("object");

                            binder.update_resource(world_location, world_cbuffer->as_const_ok().resource.buffer);
                            binder.update_resource(object_location, object_sbuffer->as_const_ok().resource.buffer);

                            binder.bind(*_device, command_list->as_const_ok().resource.command_list);

                            ResourcePtr<GeometryBuffer> geometry_buffer = _geometry_cache.get(*_upload_manager, batch.mesh->surfaces()[batch.surface_index]);
                                
                            if(geometry_buffer->is_ok()) {
                                geometry_buffer->as_ok().resource.bind(*_device, command_list->as_const_ok().resource.command_list);
                                _device->draw_indexed(command_list->as_const_ok().resource.command_list, static_cast<uint32_t>(geometry_buffer->as_ok().resource.index_size / sizeof(uint32_t)), static_cast<uint32_t>(batch.instances.size()), 0);
                            }
                        }

                        _device->close_command_list(command_list->as_const_ok().resource.command_list);
                    }
                );
            }

            return TaskCreateInfo { TaskExecution::Multithreading, std::move(command_lists) };
        }
    );

    auto final_color_info = CreateColorInfo {};
    if(_render_camera->render_target()) {
        auto gpu_texture = _rt_texture_caches->at(frame_index).get(*_render_camera->render_target());
        final_color_info.attachment = gpu_texture;
    } else {
        final_color_info.attachment = _final_images.at(frame_index);
        final_color_info.load_op = backend::RenderPassLoadOp::Clear;
        final_color_info.clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f);
    };

    auto gbuffer_input_infos = std::array<CreateInputInfo, 4> {
        CreateInputInfo { .attachment = _gbuffers.at(frame_index).positions },
        CreateInputInfo { .attachment = _gbuffers.at(frame_index).albedo },
        CreateInputInfo { .attachment = _gbuffers.at(frame_index).normals },
        CreateInputInfo { .attachment = _gbuffers.at(frame_index).roughness_metalness_ao }
    };

    frame_graph.add_pass(
        "deffered",
        _width,
        _height,
        std::span<CreateColorInfo const>(&final_color_info, 1),
        gbuffer_input_infos,
        std::nullopt,
        [&, world_cbuffer, point_light_sbuffer, frame_index](RenderPassContext& context) -> TaskCreateInfo {

            auto command_lists = context.single_for(
                [&](backend::Handle<backend::CommandList> const& command_list, backend::Handle<backend::RenderPass> const& render_pass) {
                    lib::ObjectPtr<Shader> shader = shader_cache.get("deffered_pc");
                    auto pipeline = pipeline_cache.get(*shader, context.render_pass->render_pass);

                    _device->bind_pipeline(command_list, pipeline);

                    ShaderBinder binder(*shader, null);

                    uint32_t const world_location = shader->location_uniform_by_name("world");
                    uint32_t const point_light_location = shader->location_uniform_by_name("point_light");
                    uint32_t const positions_location = shader->location_uniform_by_name("positions");
                    uint32_t const normals_location = shader->location_uniform_by_name("normals");
                    uint32_t const albedo_location = shader->location_uniform_by_name("albedo");
                    uint32_t const roughness_metalness_ao_location = shader->location_uniform_by_name("roughness_metalness_ao");

                    binder.update_resource(world_location, world_cbuffer->as_const_ok().buffer);
                    binder.update_resource(point_light_location, point_light_sbuffer->as_const_ok().buffer);
                    binder.update_resource(positions_location, _gbuffers.at(frame_index).positions->as_const_ok().texture);
                    binder.update_resource(positions_location + 1, _gbuffers.at(frame_index).positions->as_const_ok().sampler);
                    binder.update_resource(albedo_location, _gbuffers.at(frame_index).albedo->as_const_ok().texture);
                    binder.update_resource(albedo_location + 1, _gbuffers.at(frame_index).albedo->as_const_ok().sampler);
                    binder.update_resource(normals_location, _gbuffers.at(frame_index).normals->as_const_ok().texture);
                    binder.update_resource(normals_location + 1, _gbuffers.at(frame_index).normals->as_const_ok().sampler);
                    binder.update_resource(roughness_metalness_ao_location, _gbuffers.at(frame_index).roughness_metalness_ao->as_const_ok().texture);
                    binder.update_resource(roughness_metalness_ao_location + 1, _gbuffers.at(frame_index).roughness_metalness_ao->as_const_ok().sampler);

                    binder.bind(*_device, command_list);

                    if(_quad->is_ok()) {
                        _quad->as_ok()->bind(*_device, command_list);
                        _device->draw_indexed(command_list, static_cast<uint32_t>(_quad->as_ok()->index_size / sizeof(uint32_t)), 1, 0);
                    }
                }
            );

            return TaskCreateInfo { TaskExecution::Single, std::move(command_lists) }; 
        }
    );

    final_color_info.load_op = backend::RenderPassLoadOp::Load;

    depth_stencil_info = CreateDepthStencilInfo {
        .attachment = _depth_stencils.at(frame_index),
        .load_op = backend::RenderPassLoadOp::Load,
        .clear_depth = 1.0f,
        .clear_stencil = 0x0
    };

    frame_graph.add_pass(
        "forward",
        _width,
        _height,
        std::span<CreateColorInfo const>(&final_color_info, 1),
        std::nullopt,
        depth_stencil_info,
        [&, frame_index, world_cbuffer, point_light_sbuffer](RenderPassContext& context) -> TaskCreateInfo {

            auto command_lists = context.single_for(
                [&](backend::Handle<backend::CommandList> const& command_list, backend::Handle<backend::RenderPass> const& render_pass) {

                    backend::Handle<backend::Pipeline> current_pipeline = backend::InvalidHandle<backend::Pipeline>();

                    for(auto const& batch : _transculent_queue) {

                        lib::ObjectPtr<Shader> shader = shader_cache.get(batch.material->passes.at("forward"));
                        auto pipeline = pipeline_cache.get(*shader, context.render_pass->render_pass);
                        
                        if(current_pipeline != pipeline) {
                            _device->bind_pipeline(command_list, pipeline);
                            current_pipeline = pipeline;
                        }

                        ShaderBinder binder(*shader, null);

                        apply_material(binder, *shader, *batch.material, command_list, frame_index);

                        std::vector<ObjectData> object_buffers;
                        for(auto const& instance : batch.instances) {
                            object_buffers.emplace_back(instance.model, instance.inverse_model);
                        }

                        ResourcePtr<GPUBuffer> object_sbuffer = _object_pools.at(frame_index).allocate();
                        _upload_manager->upload_buffer_data(object_sbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(object_buffers.data()), object_buffers.size() * sizeof(ObjectData)));

                        uint32_t const world_location = shader->location_uniform_by_name("world");
                        uint32_t const object_location = shader->location_uniform_by_name("object");
                        uint32_t const point_light_location = shader->location_uniform_by_name("point_light");

                        binder.update_resource(world_location, world_cbuffer->as_const_ok().buffer);
                        binder.update_resource(object_location, object_sbuffer->as_const_ok().buffer);
                        binder.update_resource(point_light_location, point_light_sbuffer->as_const_ok().buffer);

                        binder.bind(*_device, command_list);

                        ResourcePtr<GeometryBuffer> geometry_buffer = _geometry_cache.get(*_upload_manager, batch.mesh->surfaces()[batch.surface_index]);
                        
                        if(geometry_buffer->is_ok()) {
                            geometry_buffer->as_ok()->bind(*_device, command_list);
                            _device->draw_indexed(command_list, static_cast<uint32_t>(geometry_buffer->as_ok()->index_size / sizeof(uint32_t)), static_cast<uint32_t>(batch.instances.size()), 0);
                        }
                    }
                }
            );
            return TaskCreateInfo { TaskExecution::Single, std::move(command_lists) }; 
        }
    );

    auto swapchain_color_info = CreateColorInfo {
        .attachment = nullptr,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
    };

    frame_graph.add_pass(
        "fxaa",
        _width,
        _height,
        std::span<CreateColorInfo const>(&swapchain_color_info, 1),
        std::nullopt,
        depth_stencil_info,
        [&, frame_index](RenderPassContext& context) -> TaskCreateInfo {

            auto command_lists = context.single_for(
                [&](backend::Handle<backend::CommandList> const& command_list, backend::Handle<backend::RenderPass> const& render_pass) {

                    lib::ObjectPtr<Shader> shader = shader_cache.get("fxaa_pc");
                    auto pipeline = pipeline_cache.get(*shader, context.render_pass->render_pass);

                    _device->bind_pipeline(command_list, pipeline);

                    ShaderBinder binder(*shader, null);

                    uint32_t const color = shader->location_uniform_by_name("color");

                    binder.update_resource(color, _final_images.at(frame_index)->as_const_ok().texture);
                    binder.update_resource(color + 1, _final_images.at(frame_index)->as_const_ok().sampler);

                    binder.bind(*_device, command_list);

                    if(_quad->is_ok()) {
                        _quad->as_ok()->bind(*_device, command_list);
                        _device->draw_indexed(command_list, static_cast<uint32_t>(_quad->as_ok().index_size / sizeof(uint32_t)), 1, 0);
                    }
                }
            );

            return TaskCreateInfo { TaskExecution::Single, std::move(command_lists) }; 
        }
    );
}

void MeshRenderer::apply_material(ShaderBinder& binder, Shader& shader, asset::Material& material, backend::Handle<backend::CommandList> const& command_list, uint32_t const frame_index) {

    std::vector<backend::MemoryBarrierDesc> memory_barriers;
    std::vector<uint8_t> material_buffer(1024);

    for(auto& [parameter_name, parameter] : material.parameters) {

        if(parameter.is_sampler2D()) {

            auto it = shader.uniforms.find(parameter_name);
            if(it == shader.uniforms.end()) {
                break;
            }

            if(parameter.as_sampler2D().asset->is_ok()) {
                ResourcePtr<GPUTexture> gpu_texture = _texture_cache.get(*_upload_manager, parameter.as_sampler2D().asset->as_ok());

                if(gpu_texture->is_ok()) {
                    uint32_t const texture_location = it->second.as_sampler2D().index;
                    binder.update_resource(texture_location, gpu_texture->as_const_ok().texture);
                    binder.update_resource(texture_location + 1, gpu_texture->as_const_ok().sampler);

                    //if(gpu_texture->as_ok().resource.memory_state.load() != backend::MemoryState::ShaderRead) {
                    //    memory_barriers.push_back(gpu_texture->as_ok().resource.barrier(backend::MemoryState::ShaderRead));
                    //}
                }
            }

        } else {
                        
            auto it = shader.uniforms.find("material");
            if(it == shader.uniforms.end()) {
                break;
            }

            ShaderUniformData<ShaderUniformType::CBuffer> const& cbuffer_data = it->second.as_cbuffer();

            auto parameter_visitor = make_visitor(
                [&](asset::MaterialParameterData<asset::MaterialParameterType::F32> const& data) {
                    std::memcpy(material_buffer.data() + cbuffer_data.offsets.at(parameter_name), &data.value, sizeof(float));
                },
                [&](asset::MaterialParameterData<asset::MaterialParameterType::F32x2> const& data) {
                    std::memcpy(material_buffer.data() + cbuffer_data.offsets.at(parameter_name), data.value.data(), sizeof(lib::math::Vector2f));
                },
                [&](asset::MaterialParameterData<asset::MaterialParameterType::F32x3> const& data) {
                    std::memcpy(material_buffer.data() + cbuffer_data.offsets.at(parameter_name), data.value.data(), sizeof(lib::math::Vector3f));
                },
                [&](asset::MaterialParameterData<asset::MaterialParameterType::F32x4> const& data) {
                    std::memcpy(material_buffer.data() + cbuffer_data.offsets.at(parameter_name), data.value.data(), sizeof(lib::math::Vector4f));
                },
                // Default
                [&](asset::MaterialParameterData<asset::MaterialParameterType::Sampler2D> const& parameter_data) { }
            );

            std::visit(parameter_visitor, parameter.data);
        }
    }

    auto it = shader.uniforms.find("material");
    if(it != shader.uniforms.end()) {
        ResourcePtr<GPUBuffer> material_cbuffer = _material_pools.at(frame_index).allocate();
        _upload_manager->upload_buffer_data(material_cbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(material_buffer.data()), material_buffer.size()));

        uint32_t const material_location = shader.location_uniform_by_name("material");
        binder.update_resource(material_location, material_cbuffer->as_const_ok().buffer);
    }

    if(!memory_barriers.empty()) {
        _device->barrier(command_list, memory_barriers);
    }
}
