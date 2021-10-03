namespace ionengine::rendersystem {

class Texture {
public:

    Texture() {
        
    }

private:

    std::unique_ptr<gfx::View> m_view;
    std::unique_ptr<gfx::Sampler> m_sampler;
    std::unique_ptr<gfx::Resource> m_resource;
};

}