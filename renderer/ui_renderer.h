// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <RmlUi/Core/RenderInterface.h>
#include <renderer/backend/backend.h>
#include <renderer/texture_cache.h>
#include <renderer/geometry_pool.h>
#include <renderer/buffer_pool.h>
#include <renderer/shader_uniform_binder.h>
#include <lib/math/matrix.h>

namespace ionengine::renderer {

__declspec(align(256)) struct UIElementData {
	lib::math::Matrixf projection;
    lib::math::Matrixf transform;
    lib::math::Vector2f translation;
};

class UiRenderer : public Rml::RenderInterface {
public:

    UiRenderer(backend::Device& device, UploadManager& upload_manager);

	void reset(uint32_t const frame_index);

	void apply_command_list(backend::Handle<backend::CommandList> const& command_list, ShaderUniformBinder& binder, ShaderProgram& shader_program, uint32_t const frame_index);

    void RenderGeometry(Rml::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::TextureHandle texture, const Rml::Vector2f& translation) override;

	void EnableScissorRegion(bool enable) override;

	void SetScissorRegion(int x, int y, int width, int height) override;

	bool LoadTexture(Rml::TextureHandle& texture_handle, Rml::Vector2i& texture_dimensions, const Rml::String& source) override;

	bool GenerateTexture(Rml::TextureHandle& texture_handle, const Rml::byte* source, const Rml::Vector2i& source_dimensions) override;

	void ReleaseTexture(Rml::TextureHandle texture_handle) override;

private:

	backend::Device* _device;
	UploadManager* _upload_manager;
	ShaderUniformBinder* _binder;
	ShaderProgram* _shader_program;

	backend::Handle<backend::CommandList> _command_list;
	uint32_t _frame_index;

	TextureCache _texture_cache;
	
	std::vector<BufferPool<BufferPoolType::CBuffer, sizeof(UIElementData)>> _ui_element_pools;
	std::vector<GeometryPool<2048, 512>> _geometry_pools;

	std::list<asset::Texture> _texture_datas;
};

}
