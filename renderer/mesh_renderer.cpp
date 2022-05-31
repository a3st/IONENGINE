// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/mesh_renderer.h>
#include <renderer/shader_binder.h>
#include <scene/scene.h>
#include <scene/camera_node.h>
#include <scene/mesh_node.h>
#include <scene/point_light_node.h>
#include <scene/scene_visitor.h>

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

        if(other.mesh().is_ok()) {
            for(size_t i = 0; i < other.mesh()->surfaces().size(); ++i) {

                lib::math::Matrixf inverse_model = other.transform_global();
                inverse_model.inverse();
                
                auto instance = SurfaceInstance {
                    .model = other.transform_global(),
                    .inverse_model = inverse_model
                };

                if(other.material(other.mesh()->surfaces()[i].material_index)->blend_mode == asset::MaterialBlendMode::Opaque) {
                    _opaque_queue->push(other.mesh(), static_cast<uint32_t>(i), instance, other.material(other.mesh()->surfaces()[i].material_index));
                } else {
                    _transculent_queue->push(other.mesh(), static_cast<uint32_t>(i), instance, other.material(other.mesh()->surfaces()[i].material_index));
                }
            }
        }
    }

    void operator()(scene::PointLightNode& other) { 

        auto point_light = PointLightData {
            .position = other.position(),
            .range = other.light_range(),
            .color = lib::math::Vector3f(other.light_color().r, other.light_color().g, other.light_color().b)
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

MeshRenderer::MeshRenderer(backend::Device& device, UploadManager& upload_manager, platform::Window& window, asset::AssetManager& asset_manager) :
    _device(&device),
    _asset_manager(&asset_manager),
    _upload_manager(&upload_manager),
    _texture_cache(device),
    _geometry_cache(device),
    _deffered_technique(asset_manager.get_technique(DEFFERED_TECHNIQUE_PATH)),
    _width(window.client_width()),
    _height(window.client_height()) {

    for(uint32_t i = 0; i < 2; ++i) {
        _object_pools.emplace_back(*_device, 32, 64, BufferPoolUsage::Dynamic);
        _world_pools.emplace_back(*_device, 4, BufferPoolUsage::Dynamic);
        _point_light_pools.emplace_back(*_device, 512, 2, BufferPoolUsage::Dynamic);

        auto gbuffer = GBufferData {
            .positions = GPUTexture::create(*_device, backend::Format::RGBA16_FLOAT, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).value(),
            .albedo = GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).value(),
            .normals = GPUTexture::create(*_device, backend::Format::RGBA16_FLOAT, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).value(),
            .roughness_metalness_ao = GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).value()
        };

        _gbuffers.emplace_back(std::move(gbuffer));
        _depth_stencils.emplace_back(GPUTexture::create(*_device, backend::Format::D32_FLOAT, window.client_width(), window.client_height(), 1, 1, backend::TextureFlags::DepthStencil).value());
    }

    auto quad_surface_data = asset::SurfaceData::make_quad();
    _quad = ResourcePtr<GeometryBuffer>(GeometryBuffer::load_from_surface(*_device, quad_surface_data).value());
    _upload_manager->upload_geometry_data(_quad, quad_surface_data.vertices.to_span(), quad_surface_data.indices.to_span());

    _deffered_technique.wait();
}

MeshRenderer::~MeshRenderer() {

}

void MeshRenderer::update(float const delta_time) {
    
}

void MeshRenderer::resize(uint32_t const width, uint32_t const height) {

    _gbuffers.clear();
    _depth_stencils.clear();

    for(uint32_t i = 0; i < 2; ++i) {
        auto gbuffer = GBufferData {
            .positions = GPUTexture::create(*_device, backend::Format::RGBA16_FLOAT, width, height, 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).value(),
            .albedo = GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, width, height, 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).value(),
            .normals = GPUTexture::create(*_device, backend::Format::RGBA16_FLOAT, width, height, 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).value(),
            .roughness_metalness_ao = GPUTexture::create(*_device, backend::Format::RGBA8_UNORM, width, height, 1, 1, backend::TextureFlags::RenderTarget | backend::TextureFlags::ShaderResource).value()
        };

        _gbuffers.emplace_back(std::move(gbuffer));
        _depth_stencils.emplace_back(GPUTexture::create(*_device, backend::Format::D32_FLOAT, width, height, 1, 1, backend::TextureFlags::DepthStencil).value());
    }
}

void MeshRenderer::render(PipelineCache& pipeline_cache, ShaderCache& shader_cache, FrameGraph& frame_graph, scene::Scene& scene, uint32_t const frame_index) {

    _object_pools.at(frame_index).reset();
    _world_pools.at(frame_index).reset();
    _point_light_pools.at(frame_index).reset();

    _opaque_queue.clear();
    _transculent_queue.clear();
    _point_lights.clear();

    _memory_barriers.clear();

    MeshVisitor mesh_visitor(_opaque_queue, _transculent_queue, _point_lights);
    scene.graph().visit(scene.graph().begin(), scene.graph().end(), mesh_visitor);

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
        [=, &pipeline_cache, &shader_cache](RenderPassContext const& context) {

            backend::Handle<backend::Pipeline> current_pipeline = backend::InvalidHandle<backend::Pipeline>();
            
            for(auto const& batch : _opaque_queue) {

                auto [pipeline, shader_program] = pipeline_cache.get(shader_cache, *batch.material->techniques.at("gbuffer"), context.render_pass);
                
                if(current_pipeline != pipeline) {
                    _device->bind_pipeline(context.command_list, pipeline);
                    current_pipeline = pipeline;
                }

                ShaderBinder binder(*shader_program);

                apply_material(binder, *shader_program, *batch.material, frame_index);

                if(!_memory_barriers.empty()) {
                    _device->barrier(context.command_list, _memory_barriers);
                    _memory_barriers.clear();
                }

                std::vector<ObjectData> object_buffers;
                for(auto const& instance : batch.instances) {
                    object_buffers.emplace_back(instance.model, instance.inverse_model);
                }

                ResourcePtr<GPUBuffer> object_sbuffer = _object_pools.at(frame_index).allocate();
                _upload_manager->upload_buffer_data(object_sbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(object_buffers.data()), object_buffers.size() * sizeof(ObjectData)));

                uint32_t const world_location = shader_program->location_uniform_by_name("world");
                uint32_t const object_location = shader_program->location_uniform_by_name("object");

                binder.update_resource(world_location, world_cbuffer->buffer);
                binder.update_resource(object_location, object_sbuffer->buffer);

                binder.bind(*_device, context.command_list);

                ResourcePtr<GeometryBuffer> geometry_buffer = _geometry_cache.get(*_upload_manager, batch.mesh->surfaces()[batch.surface_index]);
                
                if(geometry_buffer.is_ok()) {
                    geometry_buffer->bind(*_device, context.command_list);
                    _device->draw_indexed(context.command_list, static_cast<uint32_t>(geometry_buffer->index_size / sizeof(uint32_t)), static_cast<uint32_t>(batch.instances.size()), 0);
                }
            }
        }
    );

    auto swapchain_color_info = CreateColorInfo {
        .attachment = nullptr,
        .load_op = backend::RenderPassLoadOp::Clear,
        .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
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
        std::span<CreateColorInfo const>(&swapchain_color_info, 1),
        gbuffer_input_infos,
        std::nullopt,
        [=, &pipeline_cache, &shader_cache](RenderPassContext const& context) {

            auto [pipeline, shader_program] = pipeline_cache.get(shader_cache, *_deffered_technique, context.render_pass);

            _device->bind_pipeline(context.command_list, pipeline);

            ShaderBinder binder(*shader_program);

            uint32_t const world_location = shader_program->location_uniform_by_name("world");
            uint32_t const point_light_location = shader_program->location_uniform_by_name("point_light");
            uint32_t const positions_location = shader_program->location_uniform_by_name("positions");
            uint32_t const normals_location = shader_program->location_uniform_by_name("normals");
            uint32_t const albedo_location = shader_program->location_uniform_by_name("albedo");
            uint32_t const roughness_metalness_ao_location = shader_program->location_uniform_by_name("roughness_metalness_ao");

            binder.update_resource(world_location, world_cbuffer->buffer);
            binder.update_resource(point_light_location, point_light_sbuffer->buffer);
            binder.update_resource(positions_location, _gbuffers.at(frame_index).positions->texture);
            // the sampler position is always 1 greater than the texture position
            binder.update_resource(positions_location + 1, _gbuffers.at(frame_index).positions->sampler);
            binder.update_resource(albedo_location, _gbuffers.at(frame_index).albedo->texture);
            // the sampler position is always 1 greater than the texture position
            binder.update_resource(albedo_location + 1, _gbuffers.at(frame_index).albedo->sampler);
            binder.update_resource(normals_location, _gbuffers.at(frame_index).normals->texture);
            // the sampler position is always 1 greater than the texture position
            binder.update_resource(normals_location + 1, _gbuffers.at(frame_index).normals->sampler);
            binder.update_resource(roughness_metalness_ao_location, _gbuffers.at(frame_index).roughness_metalness_ao->texture);
            // the sampler position is always 1 greater than the texture position
            binder.update_resource(roughness_metalness_ao_location + 1, _gbuffers.at(frame_index).roughness_metalness_ao->sampler);

            binder.bind(*_device, context.command_list);

            if(_quad.is_ok()) {
                _quad->bind(*_device, context.command_list);
                _device->draw_indexed(context.command_list, static_cast<uint32_t>(_quad->index_size / sizeof(uint32_t)), 1, 0);
            }
        }
    );

    swapchain_color_info = CreateColorInfo {
        .attachment = nullptr,
        .load_op = backend::RenderPassLoadOp::Load,
        .clear_color = lib::math::Color(0.5f, 0.5f, 0.5f, 1.0f)
    };

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
        std::span<CreateColorInfo const>(&swapchain_color_info, 1),
        std::nullopt,
        depth_stencil_info,
        [=, &pipeline_cache, &shader_cache](RenderPassContext const& context) {

            backend::Handle<backend::Pipeline> current_pipeline = backend::InvalidHandle<backend::Pipeline>();

            for(auto const& batch : _transculent_queue) {

                auto [pipeline, shader_program] = pipeline_cache.get(shader_cache, *batch.material->techniques.at("forward"), context.render_pass);
                
                if(current_pipeline != pipeline) {
                    _device->bind_pipeline(context.command_list, pipeline);
                    current_pipeline = pipeline;
                }
                
                ShaderBinder binder(*shader_program);

                apply_material(binder, *shader_program, *batch.material, frame_index);

                if(!_memory_barriers.empty()) {
                    _device->barrier(context.command_list, _memory_barriers);
                    _memory_barriers.clear();
                }

                std::vector<ObjectData> object_buffers;
                for(auto const& instance : batch.instances) {
                    object_buffers.emplace_back(instance.model, instance.inverse_model);
                }

                ResourcePtr<GPUBuffer> object_sbuffer = _object_pools.at(frame_index).allocate();
                _upload_manager->upload_buffer_data(object_sbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(object_buffers.data()), object_buffers.size() * sizeof(ObjectData)));

                uint32_t const world_location = shader_program->location_uniform_by_name("world");
                uint32_t const object_location = shader_program->location_uniform_by_name("object");

                binder.update_resource(world_location, world_cbuffer->buffer);
                binder.update_resource(object_location, object_sbuffer->buffer);

                binder.bind(*_device, context.command_list);

                ResourcePtr<GeometryBuffer> geometry_buffer = _geometry_cache.get(*_upload_manager, batch.mesh->surfaces()[batch.surface_index]);
                
                if(geometry_buffer.is_ok()) {
                    geometry_buffer->bind(*_device, context.command_list);
                    _device->draw_indexed(context.command_list, static_cast<uint32_t>(geometry_buffer->index_size / sizeof(uint32_t)), static_cast<uint32_t>(batch.instances.size()), 0);
                }
            }
        }
    );
}

void MeshRenderer::apply_material(ShaderBinder& binder, ShaderProgram& shader_program, asset::Material& material, uint32_t const frame_index) {

    std::vector<uint8_t> material_buffer(1024);

    for(auto& [parameter_name, parameter] : material.parameters) {

        if(parameter.is_sampler2D()) {

            auto it = shader_program.uniforms.find(parameter_name);
            if(it == shader_program.uniforms.end()) {
                break;
            }

            if(parameter.as_sampler2D().asset.is_ok()) {

                ResourcePtr<GPUTexture> gpu_texture = _texture_cache.get(*_upload_manager, *parameter.as_sampler2D().asset);

                if(gpu_texture.is_ok()) {
                    
                    uint32_t const texture_location = it->second.as_sampler2D().index;
                    binder.update_resource(texture_location, gpu_texture->texture);
                    // the sampler position is always 1 greater than the texture position
                    binder.update_resource(texture_location + 1, gpu_texture->sampler);

                    if(gpu_texture->memory_state != backend::MemoryState::ShaderRead) {
                        _memory_barriers.push_back(gpu_texture->barrier(backend::MemoryState::ShaderRead));
                    }
                }
            }

        } else {
                        
            auto it = shader_program.uniforms.find("material");
            if(it == shader_program.uniforms.end()) {
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

    auto it = shader_program.uniforms.find("material");
    if(it != shader_program.uniforms.end()) {
        ResourcePtr<GPUBuffer> material_cbuffer = _material_pools.at(frame_index).allocate();
        _upload_manager->upload_buffer_data(material_cbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(material_buffer.data()), material_buffer.size()));

        uint32_t const material_location = shader_program.location_uniform_by_name("material");
        binder.update_resource(material_location, material_cbuffer->buffer);
    }
}
