namespace ionengine::rendersystem {

class Texture {
public:

    enum class Type {
        _2D,
        Cube
    };

    enum class Usage {
        Default,
        RenderTarget,
        Swapchain,
        DepthStencil
    };

    enum class Format {
        RGBA8,
        DXT1,
        DXT5,
        D32
    };

    Texture(gfx::Device* device, const std::string& name) 
        : 
            m_device(device), 
            m_name(name),
            m_width(0),
            m_height(0),
            m_usage(Texture::Usage::Default) {   

    }

    [[nodiscard]] lib::Expected<Texture*, std::string> create_from_swapchain(const uint32 buffer_index) {

        if(buffer_index + 1 > m_device->get_swapchain_buffer_size()) {
            return lib::make_expected<Texture*, std::string>("<Texture> Buffer size value is bigger than swapchain has");
        }

        m_resource = m_device->create_swapchain_resource(buffer_index);

        auto& resource_desc = m_resource->get_desc();
        m_width = static_cast<uint32>(resource_desc.width);
        m_height = resource_desc.height;

        m_usage = Texture::Usage::Swapchain;
        m_format = Texture::Format::RGBA8;

        gfx::ViewDesc view_desc{};
        view_desc.dimension = gfx::ViewDimension::Texture2D;

        m_view = m_device->create_view(gfx::ViewType::RenderTarget, m_resource.get(), view_desc);

        return lib::make_expected<Texture*, std::string>(this);
    }

    const std::string& get_name() const { return m_name; }

    uint32 get_width() const { return m_width; }
    
    uint32 get_height() const { return m_height; }

    gfx::View* get_view() const { return m_view.get(); }

    gfx::Resource* get_resource() const { return m_resource.get(); }

    RenderTexture::Usage get_usage() const { return m_usage; }

private:

    const std::string m_name;

    gfx::Device* m_device;

    std::unique_ptr<gfx::View> m_view;
    std::unique_ptr<gfx::Sampler> m_sampler;
    std::unique_ptr<gfx::Resource> m_resource;

    uint32 m_width;
    uint32 m_height;

    Texture::Usage m_usage;
    Texture::Format m_format;
};

}