namespace ionengine::rendersystem {

class Buffer {
public:

    enum class Type {

    };

    enum class Usage {
        Vertex,
        Index,
        Constant,
        Stage
    };

    Buffer(gfx::Device* device) : m_device(device) {

        assert(device && "pointer to device is null");
    }

    [[nodiscard]] lib::Expected<Buffer*, std::string> create_index_buffer(const usize size) {

        gfx::ResourceDesc resource_desc{};
        resource_desc.dimension = gfx::ViewDimension::Buffer;
        resource_desc.width = size;
        resource_desc.height = 1;
        resource_desc.mip_levels = 1;
        resource_desc.array_size = 1;
        resource_desc.flags = gfx::ResourceFlags::IndexBuffer | gfx::ResourceFlags::CopyDest;

        m_resource = m_device->create_resource(gfx::ResourceType::Buffer, gfx::MemoryType::Default, resource_desc);

        m_size = resource_desc.width;

        m_usage = Buffer::Usage::Index;
        return lib::make_expected<Buffer*, std::string>(this);
    }

    [[nodiscard]] lib::Expected<Buffer*, std::string> create_stage_buffer(const usize size) {

        gfx::ResourceDesc resource_desc{};
        resource_desc.dimension = gfx::ViewDimension::Buffer;
        resource_desc.width = size;
        resource_desc.height = 1;
        resource_desc.mip_levels = 1;
        resource_desc.array_size = 1;
        resource_desc.flags = gfx::ResourceFlags::CopySource;

        m_resource = m_device->create_resource(gfx::ResourceType::Buffer, gfx::MemoryType::Upload, resource_desc);

        m_size = resource_desc.width;

        m_usage = Buffer::Usage::Stage;
        return lib::make_expected<Buffer*, std::string>(this);
    }

    [[nodiscard]] lib::Expected<Buffer*, std::string> create_vertex_buffer(const usize size) {

        gfx::ResourceDesc resource_desc{};
        resource_desc.dimension = gfx::ViewDimension::Buffer;
        resource_desc.width = size;
        resource_desc.height = 1;
        resource_desc.mip_levels = 1;
        resource_desc.array_size = 1;
        resource_desc.flags = gfx::ResourceFlags::VertexBuffer | gfx::ResourceFlags::CopyDest;

        m_resource = m_device->create_resource(gfx::ResourceType::Buffer, gfx::MemoryType::Default, resource_desc);

        m_size = resource_desc.width;

        m_usage = Buffer::Usage::Vertex;
        return lib::make_expected<Buffer*, std::string>(this);
    }

    usize get_size() const { return m_size; }

    gfx::View* get_view() const { return m_view.get(); }

    gfx::Resource* get_resource() const { return m_resource.get(); }

    Buffer::Usage get_usage() const { return m_usage; }

private:

    gfx::Device* m_device;

    std::unique_ptr<gfx::View> m_view;
    std::unique_ptr<gfx::Resource> m_resource;

    usize m_size;

    Buffer::Usage m_usage;
};

}