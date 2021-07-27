

namespace ionengine::renderer
{

enum class ShaderType {
    Vertex = 0,
    Fragment = 1
};

class Shader final {
public:

    Shader(const Device& device, const ShaderType shader_type, const std::filesystem::path& file_path) : m_device(device), m_type(shader_type)  {

        {
            std::ifstream file(file_path, std::ios::ate | std::ios::binary);
            if (!file.is_open()) {
                std::runtime_error("Cannot open the file!");
            }

            size_t fileSize = static_cast<size_t>(file.tellg());
            m_bytes_data.resize(fileSize);

            file.seekg(std::ios::beg);
            file.read(m_bytes_data.data(), fileSize);
            
        }

        VkShaderModuleCreateInfo shader_info = {};
        shader_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_info.pCode = reinterpret_cast<const uint32*>(m_bytes_data.data());
        shader_info.codeSize = m_bytes_data.size();

        throw_if_failed(vkCreateShaderModule(m_device.get_handle(), &shader_info, nullptr, &m_handle));
    } 

    ~Shader() {

        vkDestroyShaderModule(m_device.get_handle(), m_handle, nullptr);
    }

    ShaderType get_type() const { return m_type; }
    const VkShaderModule& get_handle() const { return m_handle; }

private:

    const Device& m_device;
    const ShaderType m_type;

    VkShaderModule m_handle;

    std::vector<char> m_bytes_data;
};

}