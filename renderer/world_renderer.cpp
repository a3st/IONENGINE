// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

#include <lib/math/vector.h>

using namespace ionengine;
using namespace ionengine::renderer;

WorldRenderer::WorldRenderer(Backend* const backend, ThreadPool* const thread_pool) : _backend(backend), _thread_pool(thread_pool) {

    auto load_file = [&](std::filesystem::path const& path, std::ios::openmode const ios) -> std::vector<char8_t> {
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

    shaders[0] = _backend->create_shader(shader_vert, BackendFlags::VertexShader);
    shaders[1] = _backend->create_shader(shader_frag, BackendFlags::PixelShader);

    buffer_vertex = _backend->create_buffer(65536, BackendFlags::HostVisible);
    buffer_index = _backend->create_buffer(65536, BackendFlags::HostVisible);

    sampler = _backend->create_sampler(Filter::MinMagMipLinear, AddressMode::Wrap, AddressMode::Wrap, AddressMode::Wrap, 1, CompareOp::Always);
    
    texture_base = _backend->create_texture(Dimension::_2D, Extent2D { 1024, 1024 }, 1, 1, Format::BC1, BackendFlags::None);

    /*_backend->begin_context(ContextType::Copy);
    _backend->barrier(texture_base, MemoryState::Common, MemoryState::CopyDst);
    _backend->copy_texture_data(texture_base, std::span<char8_t const>(dds_loader.data()->data.data(), dds_loader.data()->data.size()));
    _backend->barrier(texture_base, MemoryState::CopyDst, MemoryState::Common);
    _backend->end_context();

    _backend->execute_context(ContextType::Copy);
    _backend->wait_context(ContextType::Copy);

    _backend->copy_buffer_data(buffer_vertex, 0, std::span<char8_t>(reinterpret_cast<char8_t*>(vertices.data()), vertices.size() * sizeof(Vertex)));
    _backend->copy_buffer_data(buffer_index, 0, std::span<char8_t>(reinterpret_cast<char8_t*>(indices.data()), indices.size() * sizeof(uint32_t)));*/

    constant_buffer = _backend->create_buffer(65536, BackendFlags::HostVisible | BackendFlags::ConstantBuffer);

    auto world_buffer = WorldBuffer { 
        Matrixf{}.translate(Vector3f(0.0f, 0.1f, 0.4f)).scale(Vector3f(1.0f, 1.0f, 1.0f)).transpose(), 
        Matrixf{}.translate(Vector3f(0.0f, -0.0f, -3.5f)).transpose(), 
        Matrixf{}.perspective(1.3f, 4 / 3, 0.0f, 100.0f).transpose()
    };

    _backend->copy_buffer_data(constant_buffer, 0, std::span<char8_t>(reinterpret_cast<char8_t*>(&world_buffer), sizeof(world_buffer)));

    descriptor_set = _backend->create_descriptor_set(desc_layout);

    std::vector<DescriptorWriteDesc> write_desc = { 
        DescriptorWriteDesc { 0, constant_buffer },
        DescriptorWriteDesc { 1, texture_base },
        DescriptorWriteDesc { 2, sampler }
    };
    
    _backend->write_descriptor_set(descriptor_set, write_desc);
}

void WorldRenderer::update() {

    Handle<Texture> texture = _backend->get_current_buffer();

    if(rpasses[frame_index] == Handle<RenderPass>()) {
        rpasses[frame_index] = _backend->create_render_pass(
            { texture },
            { RenderPassColorDesc { RenderPassLoadOp::Clear, RenderPassStoreOp::Store } },
            {}, {}
        );

        pipelines[frame_index] = _backend->create_pipeline(
            desc_layout,
            MeshData::vertex_declaration,
            shaders,
            RasterizerDesc { FillMode::Solid, CullMode::Back },
            DepthStencilDesc { CompareOp::Always, false },
            BlendDesc { false, Blend::One, Blend::Zero, BlendOp::Add, Blend::One, Blend::Zero, BlendOp::Add },
            rpasses[frame_index]
        );
    }

    _backend->begin_context(ContextType::Graphics);
    _backend->set_viewport(0, 0, 800, 600);
    _backend->set_scissor(0, 0, 800, 600);
    _backend->barrier(texture, MemoryState::Present, MemoryState::RenderTarget);
    _backend->barrier(texture_base, MemoryState::Common, MemoryState::PixelShaderResource);
    std::vector<Color> rtv_clears = { Color(0.2f, 0.1f, 0.3f, 1.0f) };
    _backend->begin_render_pass(rpasses[frame_index], rtv_clears, 0.0f, 0x0);
    /*_backend->bind_pipeline(pipelines[frame_index]);
    _backend->bind_descriptor_set(descriptor_set);
    _backend->bind_vertex_buffer(0, buffer_vertex, 0);
    _backend->bind_index_buffer(Format::R32, buffer_index, 0);
    _backend->draw_indexed(index_count, 1, 0);*/
    _backend->end_render_pass();
    _backend->barrier(texture, MemoryState::RenderTarget, MemoryState::Present);
    _backend->barrier(texture_base, MemoryState::PixelShaderResource, MemoryState::Common);
    _backend->end_context();

    _backend->execute_context(ContextType::Graphics);

    _backend->swap_buffers();

    frame_index = (frame_index + 1) % 2;
}

void WorldRenderer::draw_mesh(uint32_t const sort_index, MeshData const* const mesh_data, Matrixf const& model) {

    _meshes[sort_index] = mesh_data;
}

void WorldRenderer::set_projection_view(Matrixf const& projection, Matrixf const& view) {

    _world_buffer.projection = Matrixf(projection).transpose();
    _world_buffer.view = Matrixf(view).transpose();
}
