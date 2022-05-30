// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

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

class UiRenderer {
public:

	UiRenderer(backend::Device& device, UploadManager& upload_manager, platform::Window& window, asset::AssetManager& asset_manager);

	~UiRenderer();

    UiRenderer(UiRenderer const&) = delete;

    UiRenderer(UiRenderer&&) = delete;

    UiRenderer& operator=(UiRenderer const&) = delete;

    UiRenderer& operator=(UiRenderer&&) = delete;

    void update(float const delta_time);

    void resize(uint32_t const width, uint32_t const height);

    void render(PipelineCache& pipeline_cache, ShaderCache& shader_cache, backend::Handle<backend::CommandList> const& command_list, uint32_t const frame_index);

private:

	backend::Device* _device;
	UploadManager* _upload_manager;

	TextureCache _texture_cache;

	std::vector<BufferPool<BufferPoolType::CBuffer, sizeof(UIElementData)>> _ui_element_pools;
	std::vector<GeometryPool<2048, 512>> _geometry_pools;
	
};

}
