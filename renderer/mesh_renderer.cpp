// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/mesh_renderer.h>
#include <renderer/descriptor_binder.h>
#include <scene/scene.h>
#include <scene/camera_node.h>
#include <scene/mesh_node.h>
#include <scene/point_light_node.h>
#include <scene/world_environment_node.h>
#include <scene/scene_visitor.h>

using namespace ionengine;
using namespace ionengine::renderer;

namespace ionengine::renderer {

class MeshVisitor : public scene::SceneVisitor {
public:

    MeshVisitor(RenderQueue& opaque_queue, RenderQueue& transculent_queue, std::vector<PointLightData>& point_lights, WorldEnvironmentData& world_environment) : 
        _opaque_queue(&opaque_queue), 
        _transculent_queue(&transculent_queue), 
        _point_lights(&point_lights),
        _world_environment(&world_environment) { 
        
    }

    void operator()(scene::MeshNode& other) {

        if(other.mesh()->is_ok()) {
            for(size_t i = 0; i < other.mesh()->get().surfaces().size(); ++i) {

                lib::math::Matrixf inverse_model = other.model_global();
                inverse_model.inverse();
                
                auto instance = SurfaceInstance {
                    .model = other.model_global(),
                    .inverse_model = inverse_model
                };

                if(other.material(other.mesh()->get().surfaces()[i].material_index)->get().blend_mode == asset::MaterialBlendMode::Opaque) {
                    _opaque_queue->push(other.mesh(), static_cast<uint32_t>(i), instance, other.material(other.mesh()->get().surfaces()[i].material_index));
                } else {
                    _transculent_queue->push(other.mesh(), static_cast<uint32_t>(i), instance, other.material(other.mesh()->get().surfaces()[i].material_index));
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

    void operator()(scene::WorldEnvironmentNode& other) {
        // _world_environment->skybox_material = other.skybox_material();
    }

private:

    RenderQueue* _opaque_queue;
    RenderQueue* _transculent_queue;
    std::vector<PointLightData>* _point_lights;
    WorldEnvironmentData* _world_environment;
};

}

MeshRenderer::MeshRenderer(backend::Device& device, UploadManager& upload_manager, platform::Window& window, asset::AssetManager& asset_manager) :
    _device(&device),
    _asset_manager(&asset_manager),
    _upload_manager(&upload_manager),
    _texture_cache(device),
    _geometry_cache(device),
    _width(window.client_width()),
    _height(window.client_height()) {

    auto [texture_sender, texture_receiver] = lib::make_channel<asset::AssetEvent<asset::Texture>>();
    _asset_manager->texture_pool().event_dispatcher().add(texture_sender);
    _texture_receiver = texture_receiver;

    auto [mesh_sender, mesh_receiver] = lib::make_channel<asset::AssetEvent<asset::Mesh>>();
    _asset_manager->mesh_pool().event_dispatcher().add(mesh_sender);
    _mesh_receiver = mesh_receiver;

    for(uint32_t i = 0; i < backend::BACKEND_BACK_BUFFER_COUNT; ++i) {
        _object_pools.emplace_back(*_device, 32, 256, BufferPoolUsage::Dynamic);
        _world_pools.emplace_back(*_device, 4, BufferPoolUsage::Dynamic);
        _light_pools.emplace_back(*_device, 64, BufferPoolUsage::Dynamic);
        _point_light_pools.emplace_back(*_device, 512, 2, BufferPoolUsage::Dynamic);
        _material_pools.emplace_back(*_device, 64, BufferPoolUsage::Dynamic);

        auto gbuffer = GBufferData {
            .positions = make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA16_FLOAT, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()),
            .albedo = make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()),
            .normals = make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA16_FLOAT, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()),
            .roughness_metalness_ao = make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok())
        };

        _gbuffers.emplace_back(std::move(gbuffer));
        _depth_stencils.emplace_back(make_resource_ptr(GPUTexture::create(*_device, backend::Format::D32_FLOAT, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::DepthStencil | backend::TextureFlags::ShaderResource).as_ok()));
        _final_images.emplace_back(make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()));
        _ssr_images.emplace_back(make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()));
    }

    auto quad_surface_data = asset::SurfaceData::make_quad();
    _quad = make_resource_ptr(GeometryBuffer::load_from_surface(*_device, quad_surface_data).as_ok());
    _upload_manager->upload_geometry_data(_quad, quad_surface_data.vertices.to_span(), quad_surface_data.indices.to_span());

    
    _cube = asset_manager.get_mesh("engine/skybox.obj");
    _cube->wait();

    _deffered_shader = asset_manager.get_shader("engine/shaders/deffered.shader");
    _deffered_shader->wait();

    _fxaa_shader = asset_manager.get_shader("engine/shaders/fxaa.shader");
    _fxaa_shader->wait();

    _ssr_shader = asset_manager.get_shader("engine/shaders/ssr.shader");
    _ssr_shader->wait();

    _skybox_shader = asset_manager.get_shader("engine/shaders/skybox.shader");
    _skybox_shader->wait();
}

MeshRenderer::~MeshRenderer() {

}

void MeshRenderer::update(float const delta_time, std::queue<UploadBatch>& upload_batches) {
    
    {
        asset::AssetEvent<asset::Texture> texture_event;

        while(_texture_receiver->try_receive(texture_event)) {
            
            auto event_visitor = make_visitor(
                [&](asset::AssetEventData<asset::Texture, asset::AssetEventType::Loaded>& data) {
                    ResourcePtr<GPUTexture> gpu_texture = _texture_cache.get(data.asset->get());
                    
                    upload_batches.emplace(
                        UploadBatch {
                            .data = UploadBatchData<GPUTexture> { .texture = gpu_texture, .asset = data.asset }
                        }
                    );
                },
                [&](asset::AssetEventData<asset::Texture, asset::AssetEventType::Unloaded>& data) {

                },
                [&](asset::AssetEventData<asset::Texture, asset::AssetEventType::Added>& data) { },
                [&](asset::AssetEventData<asset::Texture, asset::AssetEventType::Removed>& data) { }
            );

            std::visit(event_visitor, texture_event.data);
        }
    }

    {
        asset::AssetEvent<asset::Mesh> mesh_event;

        while(_mesh_receiver->try_receive(mesh_event)) {
            
            auto event_visitor = make_visitor(
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Loaded>& data) {

                    for(size_t i = 0; i < data.asset->get().surfaces().size(); ++i) {

                        ResourcePtr<GeometryBuffer> geometry_buffer = _geometry_cache.get(data.asset->get().surfaces()[i]);
                        
                        upload_batches.emplace(
                            UploadBatch {
                                .data = UploadBatchData<GeometryBuffer> { .geometry_buffer = geometry_buffer, .asset = data.asset, .surface_index = static_cast<uint32_t>(i) }
                            }
                        );
                    }
                },
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Unloaded>& data) {

                },
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Added>& data) { },
                [&](asset::AssetEventData<asset::Mesh, asset::AssetEventType::Removed>& data) { }
            );

            std::visit(event_visitor, mesh_event.data);
        }
    }
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
        _depth_stencils.emplace_back(make_resource_ptr(GPUTexture::create(*_device, backend::Format::D32_FLOAT, width, height, 1, 1, backend::TextureFlags::DepthStencil | backend::TextureFlags::ShaderResource).as_ok()));
        _final_images.emplace_back(make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, width, height, 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()));
        _ssr_images.emplace_back(make_resource_ptr(GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, width, height, 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).as_ok()));
    }
}

void MeshRenderer::render(PipelineCache& pipeline_cache, ShaderCache& shader_cache, RTTextureCache& rt_texture_cache, NullData& null, FrameGraph& frame_graph, scene::Scene& scene, ResourcePtr<GPUTexture> swap_texture, uint32_t const frame_index) {

    _object_pools.at(frame_index).reset();
    _world_pools.at(frame_index).reset();
    _light_pools.at(frame_index).reset();
    _point_light_pools.at(frame_index).reset();
    _material_pools.at(frame_index).reset();

    _opaque_queue.clear();
    _transculent_queue.clear();
    _point_lights.clear();

    MeshVisitor mesh_visitor(_opaque_queue, _transculent_queue, _point_lights, _world_environment);
    scene.visit_culling_nodes(mesh_visitor);

    _render_camera = scene.graph().find_by_name<scene::CameraNode>("main_camera");

    auto world_environment = scene.graph().find_by_name<scene::WorldEnvironmentNode>("world_environment");
    _world_environment.skybox_material = world_environment->skybox_material();

    _render_camera->calculate_matrices();
    _opaque_queue.sort();
    _transculent_queue.sort();

    auto world_buffer = WorldData {
        .view = _render_camera->transform_view(),
        .proj = _render_camera->transform_projection(),
        .camera_position = _render_camera->position(),
        .inverse_view_proj = (_render_camera->transform_view() * _render_camera->transform_projection()).inverse()
    };

    ResourcePtr<GPUBuffer> world_cbuffer = _world_pools.at(frame_index).allocate();
    _upload_manager->upload_buffer_data(world_cbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&world_buffer), sizeof(WorldData)));

    auto light_buffer = LightData {
        .point_light_count = static_cast<uint32_t>(_point_lights.size())
    };

    ResourcePtr<GPUBuffer> light_cbuffer = _light_pools.at(frame_index).allocate();
    _upload_manager->upload_buffer_data(light_cbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&light_buffer), sizeof(LightData)));

    ResourcePtr<GPUBuffer> point_light_sbuffer = _point_light_pools.at(frame_index).allocate();
    _upload_manager->upload_buffer_data(point_light_sbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(_point_lights.data()), _point_lights.size() * sizeof(PointLightData)));

    auto depth_stencil_info = RenderPassDepthStencilInfo {
        .texture = _depth_stencils.at(frame_index),
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_depth = 1.0f,
        .clear_stencil = 0x0
    };

    auto gbuffer_color_infos = std::array<RenderPassColorInfo, 4> {
        RenderPassColorInfo {
            .texture = _gbuffers.at(frame_index).positions,
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.0f, 0.0f, 0.0f, 1.0f)
        },
        RenderPassColorInfo {
            .texture = _gbuffers.at(frame_index).albedo,
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
        },
        RenderPassColorInfo {
            .texture = _gbuffers.at(frame_index).normals,
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.0f, 0.0f, 0.0f, 1.0f)
        },
        RenderPassColorInfo {
            .texture = _gbuffers.at(frame_index).roughness_metalness_ao,
            .load_op = backend::RenderPassLoadOp::Clear,
            .clear_color = lib::math::Color(0.0f, 0.0f, 0.0f, 1.0f)
        }
    };

    frame_graph.add_pass(
        "gbuffer",
        _width,
        _height,
        gbuffer_color_infos,
        std::span<ResourcePtr<GPUTexture> const>(),
        depth_stencil_info,
        [&, frame_index, world_cbuffer](RenderPassContext& context) -> PassTaskResult {
            
            // Divide render queue to chunks if draw data larger than 16.
            // This stage allows you to upload tasks to the thread pool.
            uint32_t const divide_count = 16;

            auto chunks = _opaque_queue | ranges::views::chunk(divide_count);
            
            // Create command lists for each chunk and resize it to chunk size.
            std::vector<ResourcePtr<CommandList>> command_lists(chunks.size());

            for(size_t i = 0; i < chunks.size(); ++i) {

                ResourcePtr<CommandList> command_list = context.command_pool->allocate();
                command_lists.at(i) = command_list;

                lib::thread_pool().push(
                    lib::TaskPriorityFlags::High, // We need High task priority because renderer should not wait for anyone.
                    [&, command_list, i, chunks]() {

                        ResourcePtr<GPUPipeline> before_gpu_pipeline = nullptr;

                        for(auto const& batch : chunks.at(i)) {

                            asset::AssetPtr<asset::Shader> shader = batch.material->get().passes.at("gbuffer");

                            ResourcePtr<GPUProgram> program = shader_cache.get(shader->get());
                            ResourcePtr<GPUPipeline> after_gpu_pipeline = pipeline_cache.get(program->get(), shader->get().draw_parameters, context.render_pass->get());
                            
                            // No need to switch state if the last batch was similar.
                            if(before_gpu_pipeline != after_gpu_pipeline) {
                                after_gpu_pipeline->get().bind(*_device, command_list->get());
                                before_gpu_pipeline = after_gpu_pipeline;
                            }

                            DescriptorBinder binder(program, null);

                            // Applying material properties.
                            // Textures and buffers that have the wrong barrier will only be applied in the next frame.
                            // This limitation allows you to achieve a minimum of thread locks.
                            apply_material(binder, program->get(), batch.material->get(), frame_index);

                            // Temporary object buffers.
                            std::vector<ObjectData> object_buffers;

                            for(auto const& instance : batch.instances) {
                                object_buffers.emplace_back(instance.model, instance.inverse_model);
                            }

                            ResourcePtr<GPUBuffer> object_sbuffer = _object_pools.at(frame_index).allocate();
                            _upload_manager->upload_buffer_data(object_sbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(object_buffers.data()), object_buffers.size() * sizeof(ObjectData)));

                            // Finding uniform indices to copy descriptors to an area visible to shaders.
                            // Search by string is slower than by number, but this allows you to create the most readable code.
                            uint32_t const world_location = program->get().index_descriptor_by_name("world");
                            uint32_t const object_location = program->get().index_descriptor_by_name("object");

                            binder.update(world_location, world_cbuffer->get());
                            binder.update(object_location, object_sbuffer->get());
                            binder.bind(*_device, command_list->get());

                            ResourcePtr<GeometryBuffer> geometry_buffer = _geometry_cache.get(batch.mesh->get().surfaces()[batch.surface_index]);
                                
                            if(geometry_buffer->is_ok()) {
                                geometry_buffer->get().bind(*_device, command_list->get());
                                _device->draw_indexed(command_list->get().command_list, static_cast<uint32_t>(geometry_buffer->get().index_size / sizeof(uint32_t)), static_cast<uint32_t>(batch.instances.size()), 0);
                            }
                        }
                        command_list->get().close(*_device);
                    }
                );
            }
            return PassTaskResult::multithread(std::move(command_lists));
        }
    );

    auto final_color_info = RenderPassColorInfo {};
    if(_render_camera->render_target()) {
        // auto gpu_texture = _rt_texture_caches->at(frame_index).get(*_render_camera->render_target());
        // final_color_info.attachment = gpu_texture;
    } else {
        final_color_info.texture = _final_images.at(frame_index);
        final_color_info.load_op = backend::RenderPassLoadOp::Clear;
        final_color_info.clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f);
    };

    auto gbuffer_input_infos = std::array<ResourcePtr<GPUTexture>, 4> {
        _gbuffers.at(frame_index).positions,
        _gbuffers.at(frame_index).albedo,
        _gbuffers.at(frame_index).normals,
        _gbuffers.at(frame_index).roughness_metalness_ao
    };

    frame_graph.add_pass(
        "deffered",
        _width,
        _height,
        std::span<RenderPassColorInfo const>(&final_color_info, 1),
        gbuffer_input_infos,
        std::nullopt,
        [&, frame_index, world_cbuffer, light_cbuffer, point_light_sbuffer](RenderPassContext& context) -> PassTaskResult {

            ResourcePtr<CommandList> command_list = context.command_pool->allocate();

            ResourcePtr<GPUProgram> program = shader_cache.get(_deffered_shader->get());
            ResourcePtr<GPUPipeline> gpu_pipeline = pipeline_cache.get(program->get(), _deffered_shader->get().draw_parameters, context.render_pass->get());

            gpu_pipeline->get().bind(*_device, command_list->get());

            DescriptorBinder binder(program, null);

            uint32_t const world_location = program->get().index_descriptor_by_name("world");
            uint32_t const light_location = program->get().index_descriptor_by_name("light");
            uint32_t const point_light_location = program->get().index_descriptor_by_name("point_light");
            uint32_t const positions_location = program->get().index_descriptor_by_name("positions");
            uint32_t const normals_location = program->get().index_descriptor_by_name("normals");
            uint32_t const albedo_location = program->get().index_descriptor_by_name("albedo");
            uint32_t const roughness_metalness_ao_location = program->get().index_descriptor_by_name("roughness_metalness_ao");

            binder.update(world_location, world_cbuffer->get());
            binder.update(light_location, light_cbuffer->get());
            binder.update(point_light_location, point_light_sbuffer->get());
            binder.update(positions_location, _gbuffers.at(frame_index).positions->get());
            binder.update(albedo_location, _gbuffers.at(frame_index).albedo->get());
            binder.update(normals_location, _gbuffers.at(frame_index).normals->get());
            binder.update(roughness_metalness_ao_location, _gbuffers.at(frame_index).roughness_metalness_ao->get());

            binder.bind(*_device, command_list->get());

            if(_quad->is_ok()) {
                _quad->get().bind(*_device, command_list->get());
                _device->draw_indexed(command_list->get().command_list, static_cast<uint32_t>(_quad->get().index_size / sizeof(uint32_t)), 1, 0);
            }

            command_list->get().close(*_device);

            return PassTaskResult::singlethread(std::move(command_list)); 
        }
    );

    final_color_info.load_op = backend::RenderPassLoadOp::Load;

    depth_stencil_info = RenderPassDepthStencilInfo {
        .texture = _depth_stencils.at(frame_index),
        .load_op = backend::RenderPassLoadOp::Load,
        .clear_depth = 1.0f,
        .clear_stencil = 0x0
    };

    frame_graph.add_pass(
        "forward",
        _width,
        _height,
        std::span<RenderPassColorInfo const>(&final_color_info, 1),
        std::span<ResourcePtr<GPUTexture> const>(),
        depth_stencil_info,
        [&, frame_index, world_cbuffer, light_cbuffer, point_light_sbuffer](RenderPassContext& context) -> PassTaskResult {

            // Divide render queue to chunks if draw data larger than 16.
            // This stage allows you to upload tasks to the thread pool.
            uint32_t const divide_count = 16;

            auto chunks = _transculent_queue | ranges::views::chunk(divide_count);
            
            // Create command lists for each chunk and resize it to chunk size.
            std::vector<ResourcePtr<CommandList>> command_lists(chunks.size());

            for(size_t i = 0; i < chunks.size(); ++i) {

                ResourcePtr<CommandList> command_list = context.command_pool->allocate();
                command_lists.at(i) = command_list;

                lib::thread_pool().push(
                    lib::TaskPriorityFlags::High, // We need High task priority because renderer should not wait for anyone.
                    [&, command_list, i, chunks]() {

                        ResourcePtr<GPUPipeline> before_gpu_pipeline = nullptr;

                        for(auto const& batch : chunks.at(i)) {

                            asset::AssetPtr<asset::Shader> shader = batch.material->get().passes.at("forward");

                            ResourcePtr<GPUProgram> program = shader_cache.get(shader->get());
                            ResourcePtr<GPUPipeline> after_gpu_pipeline = pipeline_cache.get(program->get(), shader->get().draw_parameters, context.render_pass->get());
                            
                            // No need to switch state if the last batch was similar.
                            if(before_gpu_pipeline != after_gpu_pipeline) {
                                after_gpu_pipeline->get().bind(*_device, command_list->get());
                                before_gpu_pipeline = after_gpu_pipeline;
                            }

                            DescriptorBinder binder(program, null);

                            // Applying material properties.
                            // Textures and buffers that have the wrong barrier will only be applied in the next frame.
                            // This limitation allows you to achieve a minimum of thread locks.
                            apply_material(binder, program->get(), batch.material->get(), frame_index);

                            std::vector<ObjectData> object_buffers;

                            for(auto const& instance : batch.instances) {
                                object_buffers.emplace_back(instance.model, instance.inverse_model);
                            }

                            ResourcePtr<GPUBuffer> object_sbuffer = _object_pools.at(frame_index).allocate();
                            _upload_manager->upload_buffer_data(object_sbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(object_buffers.data()), object_buffers.size() * sizeof(ObjectData)));

                            uint32_t const world_location = program->get().index_descriptor_by_name("world");
                            uint32_t const light_location = program->get().index_descriptor_by_name("light");
                            uint32_t const object_location = program->get().index_descriptor_by_name("object");
                            uint32_t const point_light_location = program->get().index_descriptor_by_name("point_light");

                            binder.update(world_location, world_cbuffer->get());
                            binder.update(light_location, light_cbuffer->get());
                            binder.update(object_location, object_sbuffer->get());
                            binder.update(point_light_location, point_light_sbuffer->get());
                            binder.bind(*_device, command_list->get());

                            ResourcePtr<GeometryBuffer> geometry_buffer = _geometry_cache.get(batch.mesh->get().surfaces()[batch.surface_index]);

                            if(geometry_buffer->is_ok()) {
                                geometry_buffer->get().bind(*_device, command_list->get());
                                _device->draw_indexed(command_list->get().command_list, static_cast<uint32_t>(geometry_buffer->get().index_size / sizeof(uint32_t)), static_cast<uint32_t>(batch.instances.size()), 0);
                            }
                        }
                        command_list->get().close(*_device);
                    }
                );
            }
            return PassTaskResult::multithread(std::move(command_lists)); 
        }
    );

    frame_graph.add_pass(
        "skybox",
        _width,
        _height,
        std::span<RenderPassColorInfo const>(&final_color_info, 1),
        std::span<ResourcePtr<GPUTexture> const>(),
        depth_stencil_info,
        [&, frame_index, world_cbuffer](RenderPassContext& context) -> PassTaskResult {

            ResourcePtr<CommandList> command_list = context.command_pool->allocate();

            asset::AssetPtr<asset::Shader> skybox_shader = _world_environment.skybox_material->get().passes.at("skybox");

            ResourcePtr<GPUProgram> program = shader_cache.get(skybox_shader->get());
            ResourcePtr<GPUPipeline> gpu_pipeline = pipeline_cache.get(program->get(), skybox_shader->get().draw_parameters, context.render_pass->get());

            gpu_pipeline->get().bind(*_device, command_list->get());

            DescriptorBinder binder(program, null);

            apply_material(binder, program->get(), _world_environment.skybox_material->get(), frame_index);

            uint32_t const world_location = program->get().index_descriptor_by_name("world");
            uint32_t const object_location = program->get().index_descriptor_by_name("object");

            binder.update(world_location, world_cbuffer->get());

            ResourcePtr<GPUBuffer> gpu_buffer = _material_pools.at(frame_index).allocate();
            
            auto object_data = ObjectData {
                .model = lib::math::Matrixf::scale(lib::math::Vector3f(16.0f, 16.0f, 16.0f)),
                .inverse_model = lib::math::Matrixf::scale(lib::math::Vector3f(16.0f, 16.0f, 16.0f)).inverse()
            };
            _upload_manager->upload_buffer_data(gpu_buffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&object_data), sizeof(ObjectData)));

            binder.update(object_location, gpu_buffer->get());

            binder.bind(*_device, command_list->get());

            ResourcePtr<GeometryBuffer> geometry_buffer = _geometry_cache.get(_cube->get().surfaces()[0]);

            if(geometry_buffer->is_ok()) {
                geometry_buffer->get().bind(*_device, command_list->get());
                _device->draw_indexed(command_list->get().command_list, static_cast<uint32_t>(geometry_buffer->get().index_size / sizeof(uint32_t)), 1, 0);
            }

            command_list->get().close(*_device);

            return PassTaskResult::singlethread(std::move(command_list)); 
        }
    );

    auto ssr_color_info = RenderPassColorInfo {
        .texture = _ssr_images.at(frame_index),
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
    };

    if(_is_ssr_enable) {

    frame_graph.add_pass(
        "ssr",
        _width,
        _height,
        std::span<RenderPassColorInfo const>(&ssr_color_info, 1),
        std::span<ResourcePtr<GPUTexture> const>(),
        std::nullopt,
        [&, frame_index, world_cbuffer](RenderPassContext& context) -> PassTaskResult {

            ResourcePtr<CommandList> command_list = context.command_pool->allocate();

            ResourcePtr<GPUProgram> program = shader_cache.get(_ssr_shader->get());
            ResourcePtr<GPUPipeline> gpu_pipeline = pipeline_cache.get(program->get(), _ssr_shader->get().draw_parameters, context.render_pass->get());

            gpu_pipeline->get().bind(*_device, command_list->get());

            DescriptorBinder binder(program, null);

            uint32_t const world_location = program->get().index_descriptor_by_name("world");
            uint32_t const positions = program->get().index_descriptor_by_name("positions");
            uint32_t const normals = program->get().index_descriptor_by_name("normals");
            uint32_t const color = program->get().index_descriptor_by_name("color");
            uint32_t const roughness_metalness_ao = program->get().index_descriptor_by_name("roughness_metalness_ao");

            binder.update(world_location, world_cbuffer->get());
            binder.update(positions, _gbuffers.at(frame_index).positions->get());
            binder.update(normals, _gbuffers.at(frame_index).normals->get());
            binder.update(color, _final_images.at(frame_index)->get());
            binder.update(roughness_metalness_ao, _gbuffers.at(frame_index).roughness_metalness_ao->get());

            binder.bind(*_device, command_list->get());

            if(_quad->is_ok()) {
                _quad->get().bind(*_device, command_list->get());
                _device->draw_indexed(command_list->get().command_list, static_cast<uint32_t>(_quad->get().index_size / sizeof(uint32_t)), 1, 0);
            }

            command_list->get().close(*_device);

            return PassTaskResult::singlethread(std::move(command_list)); 
        }
    );

    }

    auto swapchain_color_info = RenderPassColorInfo {
        .texture = swap_texture,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
    };

    frame_graph.add_pass(
        "fxaa",
        _width,
        _height,
        std::span<RenderPassColorInfo const>(&swapchain_color_info, 1),
        std::span<ResourcePtr<GPUTexture> const>(),
        std::nullopt,
        [&, frame_index](RenderPassContext& context) -> PassTaskResult {

            ResourcePtr<CommandList> command_list = context.command_pool->allocate();

            ResourcePtr<GPUProgram> program = shader_cache.get(_fxaa_shader->get());
            ResourcePtr<GPUPipeline> gpu_pipeline = pipeline_cache.get(program->get(), _fxaa_shader->get().draw_parameters, context.render_pass->get());

            gpu_pipeline->get().bind(*_device, command_list->get());

            DescriptorBinder binder(program, null);

            uint32_t const color = program->get().index_descriptor_by_name("color");

            if(_is_ssr_enable) {
            binder.update(color, _ssr_images.at(frame_index)->get());
            } else {
            binder.update(color, _final_images.at(frame_index)->get());
            }
            binder.bind(*_device, command_list->get());

            if(_quad->is_ok()) {
                _quad->get().bind(*_device, command_list->get());
                _device->draw_indexed(command_list->get().command_list, static_cast<uint32_t>(_quad->get().index_size / sizeof(uint32_t)), 1, 0);
            }

            command_list->get().close(*_device);

            return PassTaskResult::singlethread(std::move(command_list)); 
        }
    );
}

void MeshRenderer::apply_material(DescriptorBinder& binder, GPUProgram const& program, asset::Material& material, uint32_t const frame_index) {

    std::vector<backend::MemoryBarrierDesc> memory_barriers;
    std::vector<uint8_t> material_buffer(1024);

    for(auto& [parameter_name, parameter] : material.parameters) {

        if(parameter.is_sampler2D()) {

            auto const it = program.descriptors.find(parameter_name);

            if(it == program.descriptors.end()) {
                break;
            }

            if(parameter.as_sampler2D().asset->is_ok()) {
                ResourcePtr<GPUTexture> gpu_texture = _texture_cache.get(parameter.as_sampler2D().asset->get());

                if(gpu_texture->is_ok()) {
                    uint32_t const texture_location = it->second.as_sampler2D().index;
                    binder.update(texture_location, gpu_texture->get());
                }
            }

        } else if(parameter.is_samplerCube()) {

            auto const it = program.descriptors.find(parameter_name);

            if(it == program.descriptors.end()) {
                break;
            }

            if(parameter.as_samplerCube().asset->is_ok()) {
                ResourcePtr<GPUTexture> gpu_texture = _texture_cache.get(parameter.as_samplerCube().asset->get());

                if(gpu_texture->is_ok()) {
                    uint32_t const texture_location = it->second.as_samplerCube().index;
                    binder.update(texture_location, gpu_texture->get());
                }
            }

        } else {
                        
            auto const it = program.descriptors.find("material");

            if(it == program.descriptors.end()) {
                break;
            }

            ProgramDescriptorData<ProgramDescriptorType::CBuffer> const& cbuffer_data = it->second.as_cbuffer();

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
                [&](asset::MaterialParameterData<asset::MaterialParameterType::Sampler2D> const& parameter_data) { },
                [&](asset::MaterialParameterData<asset::MaterialParameterType::SamplerCube> const& parameter_data) { }
            );

            std::visit(parameter_visitor, parameter.data);
        }
    }

    auto const it = program.descriptors.find("material");

    if(it != program.descriptors.end()) {
        ResourcePtr<GPUBuffer> material_cbuffer = _material_pools.at(frame_index).allocate();
        _upload_manager->upload_buffer_data(material_cbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(material_buffer.data()), material_buffer.size()));

        uint32_t const material_location = program.index_descriptor_by_name("material");
        binder.update(material_location, material_cbuffer->get());
    }
}
