// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <RmlUi/Core/RenderInterface.h>
#include <renderer/buffer_pool.h>
#include <renderer/geometry_pool.h>

namespace ionengine::renderer {

class UploadManager;
class UiRenderer;
class TextureCache;

}

namespace ionengine::ui {

class RenderInterface : public Rml::RenderInterface {

    friend class renderer::UiRenderer;

public:

    RenderInterface(renderer::backend::Device& device, renderer::UploadManager& upload_manager);

    void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) override;

	void EnableScissorRegion(bool enable) override;

	void SetScissorRegion(int x, int y, int width, int height) override;

	bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) override;

	bool GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) override;

	void ReleaseTexture(Rml::TextureHandle texture_handle) override;

private:

    renderer::backend::Device* _device;
    renderer::UploadManager* _upload_manager;

    renderer::TextureCache* _texture_cache;
    renderer::BufferPool<renderer::BufferPoolType::CBuffer, 256>* _ui_element_pool;
    renderer::GeometryPool<2048, 512>* _geometry_pool;

	renderer::backend::Handle<renderer::backend::CommandList> _command_list;

	std::list<asset::Texture> _texture_handles;
};

}
