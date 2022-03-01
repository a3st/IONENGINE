// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include <precompiled.h>
#include <renderer/world_renderer.h>

#include <lib/math/matrix.h>
#include <lib/math/vector.h>
#include <engine/obj_loader.h>
#include <engine/dds_loader.h>

using namespace ionengine;
using namespace ionengine::renderer;

struct Vertex {
    Vector3f position;
    Vector2f uv;
    Vector3f normal;

    bool operator==(Vertex const& other) const {
        return std::tie(position, uv, normal) == std::tie(other.position, other.uv, other.normal);
    }
};

struct WorldBuffer {
    Matrixf m;
    Matrixf v;
    Matrixf p;
};

namespace std {
    template<> struct hash<Vertex> {
        size_t operator()(Vertex const& other) const {
            return hash<Vector3f>()(other.position) ^ hash<Vector2f>()(other.uv) ^ hash<Vector3f>()(other.normal);
        }
    };
}

WorldRenderer::WorldRenderer(Backend* const backend, ThreadPool* const thread_pool) : _backend(backend), _thread_pool(thread_pool) {

    auto load_file = [&](std::filesystem::path const& path, std::ios::openmode const ios) -> std::vector<char8_t> {

        std::ifstream ifs(path, ios);
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

    auto mesh_data = load_file("objects/house.obj", std::ios::beg);

    std::unordered_map<Vertex, uint32_t> uniq_vertices;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    ObjLoader obj_loader;
    obj_loader.parse(std::span<char8_t>(reinterpret_cast<char8_t*>(mesh_data.data()), mesh_data.size()));

    for(auto it = obj_loader.begin(); it != obj_loader.end(); ++it) {
        std::cout << "Mesh" << std::endl;
        for(auto& index : it->indices) {

            auto vertex = Vertex {
                Vector3f(it->vertices[index.vertex].x, it->vertices[index.vertex].y, it->vertices[index.vertex].z),
                Vector2f(it->uvs[index.uv].x, it->uvs[index.uv].y),
                Vector3f(it->normals[index.normal].x, it->normals[index.normal].y, it->normals[index.normal].z)
            };

            if(uniq_vertices.count(vertex) == 0) {

                uniq_vertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.emplace_back(vertex);
            }

            indices.emplace_back(uniq_vertices[vertex]);
        }
    }

    index_count = static_cast<uint32_t>(indices.size());

    auto texture_data = load_file("textures/base_tex.dds", std::ios::binary);

    DDSLoader dds_loader;
    dds_loader.parse(texture_data);


    rpasses.resize(2);
    pipelines.resize(2);
    shaders.resize(2);

    std::vector<DescriptorRangeDesc> ranges = {
        DescriptorRangeDesc { DescriptorRangeType::ConstantBuffer, 0, 1, BackendFlags::VertexShader },
        DescriptorRangeDesc { DescriptorRangeType::ShaderResource, 1, 1, BackendFlags::PixelShader },
        DescriptorRangeDesc { DescriptorRangeType::Sampler, 2, 1, BackendFlags::PixelShader }
    };

    desc_layout = _backend->create_descriptor_layout(ranges);

    auto shader_vert = load_file("shaders/basic_vert.bin", std::ios::binary);
    auto shader_frag = load_file("shaders/basic_frag.bin", std::ios::binary);

    shaders[0] = _backend->create_shader(shader_vert, BackendFlags::VertexShader);
    shaders[1] = _backend->create_shader(shader_frag, BackendFlags::PixelShader);

    buffer_vertex = _backend->create_buffer(65536, BackendFlags::HostVisible);
    buffer_index = _backend->create_buffer(65536, BackendFlags::HostVisible);

    sampler = _backend->create_sampler(Filter::Anisotropic, AddressMode::Wrap, AddressMode::Wrap, AddressMode::Wrap, 1, CompareOp::Always);
    
    texture_base = _backend->create_texture(Dimension::_2D, Extent2D { 512, 512 }, 1, 1, Format::BC1, BackendFlags::None);

    _backend->copy_buffer_data(buffer_vertex, 0, std::span<char8_t>(reinterpret_cast<char8_t*>(vertices.data()), vertices.size() * sizeof(Vertex)));
    _backend->copy_buffer_data(buffer_index, 0, std::span<char8_t>(reinterpret_cast<char8_t*>(indices.data()), indices.size() * sizeof(uint32_t)));

    constant_buffer = _backend->create_buffer(65536, BackendFlags::HostVisible | BackendFlags::ConstantBuffer);

    auto world_buffer = WorldBuffer { 
        Matrixf{}.translate(Vector3f(0.0f, 0.1f, 0.4f)).scale(Vector3f(0.1f, 0.1f, 0.1f)).transpose(), 
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

    Handle<Texture> texture = _backend->begin_frame();

    if(rpasses[frame_index] == Handle<RenderPass>()) {
        rpasses[frame_index] = _backend->create_render_pass(
            { texture },
            { RenderPassColorDesc { RenderPassLoadOp::Clear, RenderPassStoreOp::Store } },
            {}, {}
        );

        std::vector<VertexInputDesc> vertex_inputs = {
            VertexInputDesc { "POSITION", 0, Format::RGB32, 0, sizeof(float) * 3 },
            VertexInputDesc { "TEXCOORD", 1, Format::RG32, 0, sizeof(float) * 2 },
            VertexInputDesc { "NORMAL", 2, Format::RGB32, 0, sizeof(float) * 3 }
            //VertexInputDesc { "TANGENT", 3, Format::RGB32, 0, sizeof(float) * 3 }
        };

        pipelines[frame_index] = _backend->create_pipeline(
            desc_layout,
            vertex_inputs,
            shaders,
            { FillMode::Solid, CullMode::Back },
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
    _backend->bind_vertex_buffer(0, buffer_vertex, 0);
    _backend->bind_index_buffer(Format::R32, buffer_index, 0);
    _backend->draw_indexed(index_count, 1, 0);
    _backend->end_render_pass();
    _backend->barrier(texture, MemoryState::RenderTarget, MemoryState::Present);

    _backend->end_frame();

    frame_index = (frame_index + 1) % 2;

    //std::cout << "frame end" << std::endl;
}