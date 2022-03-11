// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

#include <lib/math/vector.h>

using namespace ionengine;
using namespace ionengine::renderer;

enum class RenderPasses : uint32_t {
    Main
};

enum class Colors : uint32_t {
    Swapchain,
    Blit
};

WorldRenderer::WorldRenderer(Backend* const backend, ThreadPool* const thread_pool) : _backend(backend), _thread_pool(thread_pool) {

    auto load_file = [&](std::filesystem::path const& path, std::ios::openmode const ios = std::ios::beg) -> std::vector<char8_t> {
        std::ifstream ifs(path, ios);
        if(!ifs.is_open()) {
            throw std::runtime_error("Can not open file");
        }
        ifs.seekg(0, std::ios::end);
        size_t size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        std::vector<char8_t> buf(size);
        ifs.read(reinterpret_cast<char*>(buf.data()), buf.size());
        return buf;
    };

    _render_passes.resize(2);
    _pipelines.resize(2);
    _shaders.resize(2);

    auto ranges = std::vector<DescriptorRangeDesc> {
        DescriptorRangeDesc { DescriptorRangeType::ConstantBuffer, 0, 1, ShaderFlags::Vertex }
        //DescriptorRangeDesc { DescriptorRangeType::ShaderResource, 1, 1, ShaderFlags::Pixel },
        //DescriptorRangeDesc { DescriptorRangeType::Sampler, 2, 1, ShaderFlags::Pixel }
    };
    _pbr_layout = _backend->create_descriptor_layout(ranges);

    auto shader_vert = load_file("shaders/basic_vert.bin", std::ios::binary);
    auto shader_frag = load_file("shaders/basic_frag.bin", std::ios::binary);

    _shaders[0] = _backend->create_shader(shader_vert, ShaderFlags::Vertex);
    _shaders[1] = _backend->create_shader(shader_frag, ShaderFlags::Pixel);

    _frames.reserve(2);
    for(uint32_t i = 0; i < 2; ++i) {
        auto frame = WorldRenderer::Frame {};
        frame.vertex_buffer = _backend->create_buffer(65536, BufferFlags::HostWrite | BufferFlags::VertexBuffer);
        frame.index_buffer = _backend->create_buffer(65536, BufferFlags::HostWrite | BufferFlags::IndexBuffer);

        _frames.emplace_back(frame);
    }

    _sampler = _backend->create_sampler(Filter::MinMagMipLinear, AddressMode::Wrap, AddressMode::Wrap, AddressMode::Wrap, 1, CompareOp::Always);
    _texture = _backend->create_texture(Dimension::_2D, 1024, 1024, 1, 1, Format::BC1, TextureFlags::ShaderResource);

    _constant_buffers.reserve(2);
    for(uint32_t i = 0; i < 2; ++i) {
        _constant_buffers.emplace_back(_backend->create_buffer(65536, BufferFlags::HostWrite | BufferFlags::ConstantBuffer));
    }

    /*auto world_buffer = WorldBuffer { 
        Matrixf{}.translate(Vector3f(0.0f, 0.1f, 0.4f)).scale(Vector3f(1.0f, 1.0f, 1.0f)).transpose(), 
        Matrixf{}.translate(Vector3f(0.0f, -0.0f, -3.5f)).transpose(), 
        Matrixf{}.perspective(1.3f, 4 / 3, 0.0f, 100.0f).transpose()
    };

    _backend->copy_buffer_data(constant_buffer, 0, std::span<char8_t>(reinterpret_cast<char8_t*>(&world_buffer), sizeof(world_buffer)));

    descriptor_set = _backend->create_descriptor_set(desc_layout);

    std::vector<DescriptorWriteDesc> write_desc = { 
        DescriptorWriteDesc { 0, constant_buffer },
        DescriptorWriteDesc { 1, _texture },
        DescriptorWriteDesc { 2, _sampler }
    };
    
    _backend->write_descriptor_set(descriptor_set, write_desc);*/

    auto start = std::chrono::high_resolution_clock::now();

    _frame_graph
        .attachment(static_cast<uint32_t>(Colors::Blit), Format::RGBA8, 768, 522)
        .external_attachment(static_cast<uint32_t>(Colors::Swapchain), Format::RGBA8, MemoryState::Present, MemoryState::Present)
        .render_pass(
            static_cast<uint32_t>(RenderPasses::Main), 
            RenderPassDesc{}
                .name(u8"MainPass")
                .rect(800, 600)
                .color(static_cast<uint32_t>(Colors::Swapchain), RenderPassLoadOp::Clear, Color(0.9f, 0.5f, 0.3f, 1.0f))
                .color(static_cast<uint32_t>(Colors::Blit), RenderPassLoadOp::Clear, Color(0.2f, 0.4f, 0.3f, 1.0f)),
            [&](Handle<renderer::RenderPass> const& render_pass, RenderPassResources const& resources) {
                
                if(_pipelines[frame_index] == INVALID_HANDLE(Pipeline)) {
                    _pipelines[frame_index] = _backend->create_pipeline(
                        _pbr_layout,
                        MeshData::vertex_declaration,
                        _shaders,
                        RasterizerDesc { FillMode::Solid, CullMode::Back },
                        DepthStencilDesc { CompareOp::Always, false },
                        BlendDesc { false, Blend::One, Blend::Zero, BlendOp::Add, Blend::One, Blend::Zero, BlendOp::Add },
                        render_pass
                    );
                }

                _backend->bind_pipeline(_pipelines[frame_index]);
                _backend->bind_vertex_buffer(0, _frames[frame_index].vertex_buffer, 0);
                _backend->bind_vertex_buffer(1, _frames[frame_index].vertex_buffer, 128);
            }
        )
        .build(*_backend, 2);

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << std::format("FrameGraph: Creation completed in {} microsec", std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()) << std::endl;
}

void WorldRenderer::update() {

    Backend& backend = *_backend;

    _frame_graph
        .bind_external_attachment(static_cast<uint32_t>(Colors::Swapchain), backend.swap_buffer())
        .execute(*_backend);

    frame_index = (frame_index + 1) % 2;
}

void WorldRenderer::resize(uint32_t const width, uint32_t const height) {

    Backend& backend = *_backend;

    _frame_graph.reset(*_backend);

    /*_frame_graph
        .render_pass()
        .build(*_backend);
    */

    backend.resize_buffers(width, height, 2);
}

void WorldRenderer::draw_mesh(uint32_t const sort_index, MeshData const* const mesh_data, Matrixf const& model) {

    _meshes[sort_index] = mesh_data;
}

void WorldRenderer::set_projection_view(Matrixf const& projection, Matrixf const& view) {

    _prev_world_buffer = _world_buffer;

    _world_buffer.projection = Matrixf(projection).transpose();
    _world_buffer.view = Matrixf(view).transpose();
}
