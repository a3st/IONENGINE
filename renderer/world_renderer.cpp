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
    Swapchain
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
        DescriptorRangeDesc { DescriptorRangeType::ConstantBuffer, 0, 1, BackendFlags::VertexShader },
        DescriptorRangeDesc { DescriptorRangeType::ShaderResource, 1, 1, BackendFlags::PixelShader },
        DescriptorRangeDesc { DescriptorRangeType::Sampler, 2, 1, BackendFlags::PixelShader }
    };
    _pbr_layout = _backend->create_descriptor_layout(ranges);

    auto shader_vert = load_file("shaders/basic_vert.bin", std::ios::binary);
    auto shader_frag = load_file("shaders/basic_frag.bin", std::ios::binary);

    _shaders[0] = _backend->create_shader(shader_vert, BackendFlags::VertexShader);
    _shaders[1] = _backend->create_shader(shader_frag, BackendFlags::PixelShader);

    _frames.reserve(2);
    for(uint32_t i = 0; i < 2; ++i) {
        auto frame = WorldRenderer::Frame {};
        frame.vertex_buffer = _backend->create_buffer(65536, BackendFlags::HostVisible);
        frame.index_buffer = _backend->create_buffer(65536, BackendFlags::HostVisible);

        _frames.emplace_back(frame);
    }

    _sampler = _backend->create_sampler(Filter::MinMagMipLinear, AddressMode::Wrap, AddressMode::Wrap, AddressMode::Wrap, 1, CompareOp::Always);
    _texture = _backend->create_texture(Dimension::_2D, Extent2D { 1024, 1024 }, 1, 1, Format::BC1, BackendFlags::None);

    /*_backend->begin_context(ContextType::Copy);
    _backend->barrier(texture_base, MemoryState::Common, MemoryState::CopyDst);
    _backend->copy_texture_data(texture_base, std::span<char8_t const>(dds_loader.data()->data.data(), dds_loader.data()->data.size()));
    _backend->barrier(texture_base, MemoryState::CopyDst, MemoryState::Common);
    _backend->end_context();

    _backend->execute_context(ContextType::Copy);
    _backend->wait_context(ContextType::Copy);

    _backend->copy_buffer_data(buffer_vertex, 0, std::span<char8_t>(reinterpret_cast<char8_t*>(vertices.data()), vertices.size() * sizeof(Vertex)));
    _backend->copy_buffer_data(buffer_index, 0, std::span<char8_t>(reinterpret_cast<char8_t*>(indices.data()), indices.size() * sizeof(uint32_t)));*/

    _constant_buffers.reserve(2);
    for(uint32_t i = 0; i < 2; ++i) {
        _constant_buffers.emplace_back(_backend->create_buffer(65536, BackendFlags::HostVisible | BackendFlags::ConstantBuffer));
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

    _frame_graph
        .external_attachment(static_cast<uint32_t>(Colors::Swapchain), Format::RGBA8, MemoryState::RenderTarget, MemoryState::Present)
        .render_pass(
            static_cast<uint32_t>(RenderPasses::Main), 
            RenderPassDesc{}
                .name("MainPass")
                .color(static_cast<uint32_t>(Colors::Swapchain), RenderPassLoadOp::Clear, Color(0.9f, 0.5f, 0.3f, 1.0f)),
            [&]() {
                
            }
        )
        .build(*_backend, 2);
}

void WorldRenderer::update() {

    _frame_graph
        .bind_external_attachment(static_cast<uint32_t>(Colors::Swapchain), _backend->get_current_buffer())
        .execute(*_backend);

    frame_index = (frame_index + 1) % 2;
}

void WorldRenderer::resize(uint32_t const width, uint32_t const height) {

    _frame_graph.reset(*_backend);

    /*_frame_graph
        .render_pass()
        .build(*_backend);
    */
}

void WorldRenderer::draw_mesh(uint32_t const sort_index, MeshData const* const mesh_data, Matrixf const& model) {

    _meshes[sort_index] = mesh_data;
}

void WorldRenderer::set_projection_view(Matrixf const& projection, Matrixf const& view) {

    _prev_world_buffer = _world_buffer;

    _world_buffer.projection = Matrixf(projection).transpose();
    _world_buffer.view = Matrixf(view).transpose();
}
