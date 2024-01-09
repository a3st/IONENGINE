// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

#include "backend.hpp"
#include "render_graph.hpp"

namespace ionengine {

namespace platform {

class Window;

}

namespace renderer {

class Renderer {
public:

    Renderer(platform::Window& window);

    Renderer(Renderer const&) = delete;

    Renderer(Renderer&&) = delete;

    auto operator=(Renderer const&) -> Renderer& = delete;

    auto operator=(Renderer&&) -> Renderer& = delete;

    auto render() -> void;

    auto resize(uint32_t const width, uint32_t const height) -> void;

private:

    Backend backend;
    core::ref_ptr<RenderGraph> render_graph{nullptr};

    /*backend::Device _device;

    UploadManager _upload_manager;
    FrameGraph _frame_graph;

    ShaderCache _shader_cache;
    PipelineCache _pipeline_cache;
    std::vector<RTTextureCache> _rt_texture_caches;

    std::vector<ResourcePtr<GPUTexture>> _swap_textures;

    std::queue<UploadBatch> _upload_batches;

    UiRenderer _ui_renderer;
    MeshRenderer _mesh_renderer;

    uint32_t _width;
    uint32_t _height;

    NullData _null;*/
};

}

}