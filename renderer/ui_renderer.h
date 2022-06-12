// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include <renderer/backend/backend.h>
#include <renderer/texture_cache.h>
#include <renderer/geometry_pool.h>
#include <renderer/buffer_pool.h>
#include <renderer/rt_texture_cache.h>
#include <renderer/shader_cache.h>
#include <renderer/frame_graph.h>
#include <renderer/pipeline_cache.h>
#include <asset/asset_manager.h>
#include <lib/math/matrix.h>

namespace ionengine {

namespace ui {
class UserInterface;
class RenderInterface;
};

}

namespace ionengine::renderer {

struct NullData;

__declspec(align(256)) struct UIElementData {
	lib::math::Matrixf projection;
    lib::math::Matrixf transform;
    lib::math::Vector2f translation;
    int32_t has_texture;
};

class UiRenderer {

    friend class ui::RenderInterface;

public:

	UiRenderer(backend::Device& device, UploadManager& upload_manager, std::vector<RTTextureCache>& rt_texture_caches, platform::Window& window, asset::AssetManager& asset_manager);

	~UiRenderer();

    UiRenderer(UiRenderer const&) = delete;

    UiRenderer(UiRenderer&&) = delete;

    UiRenderer& operator=(UiRenderer const&) = delete;

    UiRenderer& operator=(UiRenderer&&) = delete;

    void update(float const delta_time);

    void resize(uint32_t const width, uint32_t const height);

    void render(PipelineCache& pipeline_cache, ShaderCache& shader_cache, NullData& null, FrameGraph& frame_graph, ui::UserInterface& ui, uint32_t const frame_index);

private:

	backend::Device* _device;
    asset::AssetManager* _asset_manager;
	UploadManager* _upload_manager;
    std::vector<RTTextureCache>* _rt_texture_caches;

	TextureCache _texture_cache;

	std::vector<BufferPool<BufferPoolType::CBuffer, sizeof(UIElementData)>> _ui_element_pools;
	std::vector<GeometryPool<8192, 2136>> _geometry_pools;

    uint32_t _width;
    uint32_t _height;
};

}
