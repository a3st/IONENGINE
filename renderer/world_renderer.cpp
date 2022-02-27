// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

#include <lib/math/matrix.h>
#include <engine/obj_loader.h>

using namespace ionengine::renderer;

WorldRenderer::WorldRenderer(Backend* const backend, ThreadPool* const thread_pool) : _backend(backend), _thread_pool(thread_pool) {

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

    auto mesh_data = load_file("objects/cube.obj");

    ObjLoader obj_loader;
    obj_loader.parse(std::span<char8_t>(reinterpret_cast<char8_t*>(mesh_data.data()), mesh_data.size()));

    for(auto it = obj_loader.begin(); it != obj_loader.end(); ++it) {
        std::cout << "Mesh loaded" << std::endl;
    }

    rpasses.resize(2);
    pipelines.resize(2);
    shaders.resize(2);

    std::vector<DescriptorRangeDesc> ranges = {
        DescriptorRangeDesc { DescriptorRangeType::ConstantBuffer, 0, 3, BackendFlags::VertexShader },
        DescriptorRangeDesc { DescriptorRangeType::ConstantBuffer, 4, 3, BackendFlags::PixelShader },
        DescriptorRangeDesc { DescriptorRangeType::ShaderResource, 0, 5, BackendFlags::PixelShader }
    };

    desc_layout = _backend->create_descriptor_layout(ranges);

    auto shader_vert = load_file("shaders/basic_vert.bin");
    auto shader_frag = load_file("shaders/basic_frag.bin");

    shaders[0] = _backend->create_shader(shader_vert, BackendFlags::VertexShader);
    shaders[1] = _backend->create_shader(shader_frag, BackendFlags::PixelShader);

    buffer_vertex = _backend->create_buffer(65536, BackendFlags::HostVisible);
    buffer_index = _backend->create_buffer(65536, BackendFlags::HostVisible);

    //std::vector<float> vertex = { -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f,  0.5f, 0.0f };
    //_backend->copy_buffer_data(buffer_vertex, 0, std::span<char8_t>(reinterpret_cast<char8_t*>(vertex.data()), vertex.size() * sizeof(float)));

    _backend->copy_buffer_data(buffer_vertex, 0, 
        std::span<char8_t>(reinterpret_cast<char8_t*>(obj_loader.begin()->vertices.data()), obj_loader.begin()->vertices.size() * sizeof(float)));

    _backend->copy_buffer_data(buffer_index, 0, 
        std::span<char8_t>(reinterpret_cast<char8_t*>(obj_loader.begin()->indices.data()), obj_loader.begin()->indices.size() * sizeof(uint32_t)));

    constant_buffer = _backend->create_buffer(65536, BackendFlags::HostVisible | BackendFlags::ConstantBuffer);

    struct WorldBuffer {
        Matrixf m;
        Matrixf v;
        Matrixf p;
    };

    auto world_buffer = WorldBuffer { Matrixf{}.identity(), Matrixf{}.identity(), Matrixf{}.identity() };

    _backend->copy_buffer_data(constant_buffer, 0, std::span<char8_t>(reinterpret_cast<char8_t*>(&world_buffer), sizeof(world_buffer)));

    descriptor_set = _backend->create_descriptor_set(desc_layout);

    std::vector<DescriptorWriteDesc> write_desc = { 
        DescriptorWriteDesc { 0, constant_buffer }
    };
    
    _backend->write_descriptor_set(descriptor_set, write_desc);
}

void WorldRenderer::update() {

    Handle<Texture> texture = _backend->begin_frame();

    if(rpasses[frame_index] == Handle<RenderPass>()) {
        rpasses[frame_index] = _backend->create_render_pass(
            { texture },
            { RenderPassColorDesc { RenderPassLoadOp::Clear, RenderPassStoreOp::Store } },
            {}, {}
        );

        std::vector<VertexInputDesc> vertex_inputs = {
            VertexInputDesc { "POSITION", 0, Format::RGB32, 0, sizeof(float) * 3 },
            VertexInputDesc { "TEXCOORD0", 1, Format::RG32, 0, sizeof(float) * 2 },
            VertexInputDesc { "NORMAL", 2, Format::RGB32, 0, sizeof(float) * 3 },
            VertexInputDesc { "TANGENT", 2, Format::RGB32, 0, sizeof(float) * 3 }
        };

        pipelines[frame_index] = _backend->create_pipeline(
            desc_layout,
            vertex_inputs,
            shaders,
            { FillMode::Solid, CullMode::Front },
            { CompareOp::Always, false },
            { false, Blend::One, Blend::Zero, BlendOp::Add, Blend::One, Blend::Zero, BlendOp::Add },
            rpasses[frame_index]
        );
    }

    _backend->set_viewport(0, 0, 800, 600);
    _backend->set_scissor(0, 0, 800, 600);
    _backend->barrier(texture, MemoryState::Present, MemoryState::RenderTarget);
    std::vector<Color> rtv_clears = { Color(0.2f, 0.1f, 0.3f, 1.0f) };
    _backend->begin_render_pass(rpasses[frame_index], rtv_clears, 0.0f, 0x0);
    _backend->bind_pipeline(pipelines[frame_index]);
    _backend->bind_descriptor_set(descriptor_set);
    _backend->bind_vertex_buffer(0, buffer_vertex);
    _backend->draw(0, 3);
    _backend->end_render_pass();
    _backend->barrier(texture, MemoryState::RenderTarget, MemoryState::Present);

    _backend->end_frame();

    frame_index = (frame_index + 1) % 2;

    //std::cout << "frame end" << std::endl;
}