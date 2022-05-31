// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <ui/render_interface.h>
#include <renderer/ui_renderer.h>
#include <renderer/shader_binder.h>

using namespace ionengine;
using namespace ionengine::ui;

void RenderInterface::RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) {

    asset::Texture* texture_data = reinterpret_cast<asset::Texture*>(texture);

    auto gpu_texture = _texture_cache->get(*_upload_manager, *texture_data);

    if(gpu_texture.is_ok()) {

        uint32_t const albedo_location = _shader_program->location_uniform_by_name("albedo");
        _binder->update_resource(albedo_location, gpu_texture->texture);
        // the sampler position is always 1 greater than the texture position
        _binder->update_resource(albedo_location + 1, gpu_texture->sampler);

        if(gpu_texture->memory_state != renderer::backend::MemoryState::ShaderRead) {
            gpu_texture->barrier(renderer::backend::MemoryState::ShaderRead);
        }
    }

    if(!memory_barriers.empty()) {
        _device->barrier(_command_list, memory_barriers);
        memory_barriers.clear();
    }

    auto ui_element_buffer = renderer::UIElementData {
        .projection = lib::math::Matrixf::orthographic_rh(0.0f, static_cast<float>(_width), static_cast<float>(_height), 0.0f, 0.0f, 1.0f),
        .transform = lib::math::Matrixf::identity(),
        .translation = lib::math::Vector2f(translation.x, translation.y)
    };

    renderer::ResourcePtr<renderer::GPUBuffer> ui_element_cbuffer = _ui_element_pool->allocate();
    _upload_manager->upload_buffer_data(ui_element_cbuffer, 0, std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(&ui_element_buffer), sizeof(renderer::UIElementData)));

    uint32_t const ui_element_location = _shader_program->location_uniform_by_name("ui_element");
    _binder->update_resource(ui_element_location, ui_element_cbuffer->buffer);
    _binder->bind(*_device, _command_list);

    uint64_t offset = 0;
    std::vector<uint8_t> vertex_data(num_vertices * (sizeof(Rml::Vector2f) + 4 * sizeof(float) + sizeof(Rml::Vector2f)));

    for(int32_t i = 0; i < num_vertices; ++i) {
        std::memcpy(vertex_data.data() + offset, &vertices[i].position, sizeof(Rml::Vector2f));
        offset += sizeof(Rml::Vector2f);

        float const r = static_cast<float>(static_cast<uint32_t>(vertices[i].colour.red) / 255);
        float const g = static_cast<float>(static_cast<uint32_t>(vertices[i].colour.green) / 255);
        float const b = static_cast<float>(static_cast<uint32_t>(vertices[i].colour.blue) / 255);
        float const a = static_cast<float>(static_cast<uint32_t>(vertices[i].colour.alpha) / 255);

        std::memcpy(vertex_data.data() + offset, &r, sizeof(float));
        offset += sizeof(float);
        std::memcpy(vertex_data.data() + offset, &g, sizeof(float));
        offset += sizeof(float);
        std::memcpy(vertex_data.data() + offset, &b, sizeof(float));
        offset += sizeof(float);
        std::memcpy(vertex_data.data() + offset, &a, sizeof(float));
        offset += sizeof(float);

        std::memcpy(vertex_data.data() + offset, &vertices[i].tex_coord, sizeof(Rml::Vector2f));
        offset += sizeof(Rml::Vector2f);
    }

    std::vector<uint8_t> index_data(num_indices * sizeof(uint32_t));
    std::memcpy(index_data.data(), indices, num_indices * sizeof(uint32_t));

    renderer::ResourcePtr<renderer::GeometryBuffer> geometry_buffer = _geometry_pool->allocate();
    _upload_manager->upload_geometry_data(geometry_buffer, vertex_data, index_data);
    
    geometry_buffer->bind(*_device, _command_list);
    _device->draw_indexed(_command_list, num_indices, 1, 0);
}

void RenderInterface::EnableScissorRegion(bool enable) {

}

void RenderInterface::SetScissorRegion(int x, int y, int width, int height) {
    _device->set_scissor(_command_list, x, y, width, height);
}

bool RenderInterface::LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) {

    return false;
}

bool RenderInterface::GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) {

    auto texture = asset::Texture {
        .data = std::span<uint8_t const>(reinterpret_cast<uint8_t const*>(source), source_dimensions.x * source_dimensions.y * 4),
        .width = static_cast<uint32_t>(source_dimensions.x),
        .height = static_cast<uint32_t>(source_dimensions.y),
        .format = asset::TextureFormat::RGBA8_UNORM,
        .filter = asset::TextureFilter::MinMagMipLinear,
        .mip_count = 1
    };
    texture.hash = texture.data.hash();

    _texture_cache->get(*_upload_manager, texture);

    texture_handle = (Rml::TextureHandle)&_texture_handles.emplace_back(std::move(texture));
    return true;
}

void RenderInterface::ReleaseTexture(Rml::TextureHandle texture_handle) {

}
