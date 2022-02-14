// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

using namespace ionengine::renderer;

WorldRenderer::WorldRenderer(Backend* const backend, ThreadPool* const thread_pool) : _backend(backend), _thread_pool(thread_pool) {

    rpasses.resize(2);
    pipelines.resize(2);
    shaders.resize(2);

    desc_layout = _backend->create_descriptor_layout({
        DescriptorBindDesc {
            DescriptorBindType::ConstantBuffer,
            0,
            1,
            ResourceFlags::VertexShader
        }
    });

    auto load_file = [&](std::filesystem::path const& path) -> std::vector<char8_t> {

        std::ifstream ifs(path);
        if(!ifs.is_open()) {
            throw std::runtime_error("Can't open shader!");
        }

        ifs.seekg(0, std::ios::end);
        size_t size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<char8_t> buf(size);
        ifs.read(reinterpret_cast<char*>(buf.data()), buf.size());
        return buf;
    };

    auto shader_vert = load_file("shaders/basic_vert.bin");
    auto shader_frag = load_file("shaders/basic_frag.bin");

    shaders[0] = _backend->create_shader(shader_vert, ResourceFlags::VertexShader);
    shaders[1] = _backend->create_shader(shader_frag, ResourceFlags::PixelShader);

    buffer_vertex = _backend->create_buffer(65536, ResourceFlags::HostVisible);

    std::vector<float> vertex = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f,  0.5f, 0.0f };

    _backend->copy_buffer_data(buffer_vertex, 0, std::span<char8_t>(reinterpret_cast<char8_t*>(vertex.data()), vertex.size() * sizeof(float)));

    constant_buffer = _backend->create_buffer(65536, ResourceFlags::HostVisible | ResourceFlags::ConstantBuffer);

    descriptor_set = _backend->create_descriptor_set(desc_layout);

    std::vector<std::variant<Handle<Texture>, Handle<Buffer>, Handle<Sampler>>> data = { constant_buffer };
    
    _backend->update_descriptor_set(
        descriptor_set,
        { { 0, data } }
    );
}

void WorldRenderer::update() {

    Handle<Texture> texture = _backend->begin_frame();

    if(rpasses[frame_index] == Handle<RenderPass>()) {
        rpasses[frame_index] = _backend->create_render_pass(
            { texture },
            { RenderPassColorDesc { RenderPassLoadOp::Clear, RenderPassStoreOp::Store } },
            {}, {}
        );

        pipelines[frame_index] = _backend->create_pipeline(
            desc_layout,
            { VertexInputDesc { "POSITION", 0, Format::RGB32, 0, 0 } },
            { shaders[0], shaders[1] },
            { FillMode::Solid, CullMode::Front },
            { CompareOp::Always, false },
            { false, Blend::One, Blend::Zero, BlendOp::Add, Blend::One, Blend::Zero, BlendOp::Add },
            rpasses[frame_index]
        );
    }

    _backend->set_viewport(0, 0, 800, 600);
    _backend->set_scissor(0, 0, 800, 600);
    _backend->barrier(texture, MemoryState::Present, MemoryState::RenderTarget);
    _backend->begin_render_pass(rpasses[frame_index], { Color(0.2f, 0.1f, 0.3f, 1.0f) }, {});
    _backend->bind_descriptor_set(descriptor_set);
    _backend->bind_pipeline(pipelines[frame_index]);
    _backend->bind_vertex_buffer(0, buffer_vertex);
    _backend->draw(0, 3);
    _backend->end_render_pass();
    _backend->barrier(texture, MemoryState::RenderTarget, MemoryState::Present);

    _backend->end_frame();

    frame_index = (frame_index + 1) % 2;

    //std::cout << "frame end" << std::endl;
}