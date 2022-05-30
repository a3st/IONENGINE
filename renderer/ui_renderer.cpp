// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/ui_renderer.h>

using namespace ionengine;
using namespace ionengine::renderer;

UiRenderer::UiRenderer(backend::Device& device, UploadManager& upload_manager) :
    _device(&device),
    _texture_cache(device),
    _upload_manager(&upload_manager) {

    for(uint32_t i = 0; i < 2; ++i) {
        _geometry_pools.emplace_back(device, 128, GeometryPoolUsage::Dynamic);
        _ui_element_pools.emplace_back(device, 128, BufferPoolUsage::Dynamic);
    }
}

void UiRenderer::reset(uint32_t const frame_index) {
    _geometry_pools.at(frame_index).reset();
    _ui_element_pools.at(frame_index).reset();
}

void UiRenderer::apply_command_list(backend::Handle<backend::CommandList> const& command_list, ShaderUniformBinder& binder, ShaderProgram& shader_program, uint32_t const frame_index) {
    _command_list = command_list;
    _binder = &binder;
    _shader_program = &shader_program;
    _frame_index = frame_index;
}

void UiRenderer::RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) {

    asset::Texture* texture_data = reinterpret_cast<asset::Texture*>(texture);

    std::vector<backend::MemoryBarrierDesc> barriers;

    auto gpu_texture = _texture_cache.get(*_upload_manager, *texture_data);
    if(gpu_texture.is_ok()) {
        uint32_t const albedo_location = _shader_program->location_uniform_by_name("albedo");
        _binder->bind_resource(albedo_location, gpu_texture->texture);
        _binder->bind_resource(albedo_location + 1, gpu_texture->sampler);

        if(gpu_texture->memory_state != backend::MemoryState::ShaderRead) {
            barriers.emplace_back(gpu_texture->texture, gpu_texture->memory_state, backend::MemoryState::ShaderRead);
            gpu_texture->memory_state = backend::MemoryState::ShaderRead;
        }
    }

    if(!barriers.empty()) {
        _device->barrier(_command_list, barriers);
    }

    auto element_data = UIElementData {
        .projection = lib::math::Matrixf::orthographic_rh(0.0f, 800, 600, 0.0f, 0.0f, 1.0f),
        .transform = lib::math::Matrixf::identity(),
        .translation = lib::math::Vector2f(translation.x, translation.y)
    };

    ResourcePtr<GPUBuffer> ui_element_buffer;
    {
        ui_element_buffer = _ui_element_pools.at(_frame_index).allocate();
        _upload_manager->upload_buffer_data(ui_element_buffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&element_data), sizeof(UIElementData)));
    }

    uint32_t const ui_element_location = _shader_program->location_uniform_by_name("ui_element");
    _binder->bind_resource(ui_element_location, ui_element_buffer->buffer);

    _binder->update(_command_list);

    // std::cout << std::format("{} vertices, {} indices", num_vertices, num_indices) << std::endl;

    std::vector<uint8_t> vertices_data(num_vertices * (sizeof(Rml::Vector2f) + 4 * sizeof(float) + sizeof(Rml::Vector2f)));

    uint64_t offset = 0;

    for(int32_t i = 0; i < num_vertices; ++i) {
        std::memcpy(vertices_data.data() + offset, &vertices[i].position, sizeof(Rml::Vector2f));
        offset += sizeof(Rml::Vector2f);
        //std::memcpy(vertices_data.data() + offset, &vertices->colour, sizeof(Rml::Colourb));
        offset += 4 * sizeof(float);
        std::memcpy(vertices_data.data() + offset, &vertices[i].tex_coord, sizeof(Rml::Vector2f));
        offset += sizeof(Rml::Vector2f);
    }

    std::vector<uint8_t> indices_data(num_indices * sizeof(uint32_t));
    std::memcpy(indices_data.data(), indices, num_indices * sizeof(uint32_t));

    auto geometry_buffer = _geometry_pools.at(_frame_index).allocate();
    _upload_manager->upload_geometry_data(geometry_buffer, vertices_data, indices_data);
    
    geometry_buffer->bind(*_device, _command_list);
    _device->draw_indexed(_command_list, num_indices, 1, 0);
}

void UiRenderer::EnableScissorRegion(bool enable) {

}

void UiRenderer::SetScissorRegion(int x, int y, int width, int height) {
    _device->set_scissor(_command_list, x, y, width, height);
}

bool UiRenderer::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) {

    return false;
}

bool UiRenderer::GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) {

    auto texture = asset::Texture {
        .data = std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(source), source_dimensions.x * source_dimensions.y * 4),
        .width = static_cast<uint32_t>(source_dimensions.x),
        .height = static_cast<uint32_t>(source_dimensions.y),
        .format = asset::TextureFormat::RGBA8_UNORM,
        .filter = asset::TextureFilter::MinMagMipLinear,
        .mip_count = 1
    };
    texture.hash = texture.data.hash();

    _texture_cache.get(*_upload_manager, texture);

    texture_handle = (Rml::TextureHandle)&_texture_datas.emplace_back(std::move(texture));
    return true;
}

void UiRenderer::ReleaseTexture(Rml::TextureHandle texture_handle) {

}
