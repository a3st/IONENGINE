#define VK_USE_PLATFORM_WIN32_KHR

#include <precompiled.h>
#include <renderer/backend.h>
#include <vulkan/vulkan.hpp>
#include <platform/window.h>

using namespace ionengine::renderer;

#ifdef _DEBUG
    constexpr bool IS_ENABLED_DEBUG = true;
#else
    constexpr bool IS_ENABLED_DEBUG = false;
#endif

constexpr std::string_view app_name = "";
constexpr std::string_view engine_name = "";
constexpr const char* required_instance_extensions[3] = {"VK_KHR_surface", "VK_KHR_win32_surface", VK_EXT_DEBUG_UTILS_EXTENSION_NAME};

VkBool32 debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, VkDebugUtilsMessengerCallbackDataEXT const * pCallbackData, void * /*pUserData*/) {
    std::string severity = "[UNKNOWN]";
    switch (messageSeverity) {
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            severity = "[ERROR]";
            break;
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            severity = "[WARNING]";
            break;
        case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            severity = "[INFO]";
            break;
        default:
            break;
    }

    std::string type = "[UNKNOWN]";
    switch (messageTypes) {
        case VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
            severity = "[GENERAL]";
            break;
        case VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
            severity = "[PERFORMANCE]";
            break;
        case VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
            severity = "[VALIDATION]";
            break;
        default:
            break;
    }

    std::cout << std::format("{} {} MESSAGE:\n{}", severity, type, pCallbackData->pMessage) << std::endl;

    return VK_FALSE;
}

struct Backend::Impl {
    vk::Instance instance;
    std::optional<vk::DebugUtilsMessengerEXT> debug_utils;

    vk::SurfaceKHR surface;

    vk::PhysicalDevice physical_device;
    vk::Device device;

    uint32_t graphics_queue_family_index;
    vk::SurfaceFormatKHR surface_format;
    vk::PresentModeKHR present_mode;

    void initialize(uint32_t const adapter_index, SwapchainDesc const& swapchain_desc, std::filesystem::path const& shader_cache_path = "");
    void deinitialize();

    void check_instance_support_extensions();
    void check_instance_supoort_layers();
    void create_device(const vk::Instance& instance, const vk::SurfaceKHR& surface);
};

void Backend::impl_deleter::operator()(Impl* ptr) const {
    delete ptr;
}

void Backend::Impl::initialize(uint32_t const adapter_index, SwapchainDesc const& swapchain_desc, std::filesystem::path const& shader_cache_path) {
    vk::ApplicationInfo application_info(app_name.data(), VK_MAKE_VERSION(0, 0, 1), engine_name.data(), VK_MAKE_VERSION(0, 0, 1), VK_VERSION_1_2);

    std::vector<const char*> instance_extensions;
    std::vector<const char*> instance_layers;
    if (IS_ENABLED_DEBUG) {
        check_instance_support_extensions();
        check_instance_supoort_layers();

        instance_extensions.insert(instance_extensions.end(), std::begin(required_instance_extensions), std::end(required_instance_extensions));
        instance_layers.push_back("VK_LAYER_KHRONOS_validation");
    }

    vk::InstanceCreateInfo instnace_create_info({}, &application_info, instance_layers, instance_extensions);
    instance = vk::createInstance(instnace_create_info);

    if (IS_ENABLED_DEBUG) {
        vk::DebugUtilsMessageSeverityFlagsEXT severity(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo);
        vk::DebugUtilsMessageTypeFlagsEXT type(vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

        debug_utils.emplace(instance.createDebugUtilsMessengerEXT(vk::DebugUtilsMessengerCreateInfoEXT({}, severity, type, &debug_callback)));
    }

    vk::Win32SurfaceCreateInfoKHR surface_info({}, GetModuleHandle(NULL), reinterpret_cast<HWND>(swapchain_desc.window->get_handle()));
    auto err = instance.createWin32SurfaceKHR(&surface_info, nullptr, &surface);
    if (err != vk::Result::eSuccess) {
        throw std::runtime_error("Failed to create Surface");
    }

    create_device(instance, surface);
}

void Backend::Impl::deinitialize() {
    if (debug_utils.has_value()) {
        instance.destroyDebugUtilsMessengerEXT(debug_utils.value());
    }

    instance.destroySurfaceKHR(surface);
    instance.destroy();
}

Backend::Backend(uint32_t const adapter_index, SwapchainDesc const& swapchain_desc, std::filesystem::path const& shader_cache_path) : _impl(std::unique_ptr<Impl, impl_deleter>(new Impl())) {
    _impl->initialize(adapter_index, swapchain_desc, shader_cache_path);
}

Backend::~Backend() {

}

void Backend::Impl::check_instance_support_extensions() {
    std::vector<vk::ExtensionProperties> instance_layer_extensions = vk::enumerateInstanceExtensionProperties();

    for (const auto& extension : required_instance_extensions) {
        if (std::find_if(instance_layer_extensions.begin(), instance_layer_extensions.end(), [&](vk::ExtensionProperties const& extension_property) {
                return strcmp(extension, extension_property.extensionName) == 0;
            }) == instance_layer_extensions.end()) {
            throw std::runtime_error(std::format("Faild to create an Instance. Unsupported Extension: \"{}\".", extension));
        }
    }
}

void Backend::Impl::check_instance_supoort_layers() {
    std::vector<vk::LayerProperties> instance_layer_properties = vk::enumerateInstanceLayerProperties();

    if (std::find_if(instance_layer_properties.begin(), instance_layer_properties.end(), [](vk::LayerProperties const& layer_property) {
            return strcmp("VK_LAYER_KHRONOS_validation", layer_property.layerName) == 0;
        }) == instance_layer_properties.end()) {
        throw std::runtime_error("Faild to create an Instance. Unsupported Layer: \"VK_LAYER_KHRONOS_validation\".");
    }
}

void Backend::Impl::create_device(const vk::Instance& instance, const vk::SurfaceKHR& surface) {
    std::vector<vk::PhysicalDevice> physical_devices = instance.enumeratePhysicalDevices();

    auto is_found = std::find_if(physical_devices.begin(), physical_devices.end(), [&](vk::PhysicalDevice physical_device) {
        auto queue_family_properites = physical_device.getQueueFamilyProperties();

        auto is_found_graphics_queue_family = std::find_if(queue_family_properites.begin(),queue_family_properites.end(), [](vk::QueueFamilyProperties const& queue_family_property) {
            return queue_family_property.queueFlags & vk::QueueFlagBits::eGraphics;
        });

        if (is_found_graphics_queue_family == queue_family_properites.end()) {
            return false;
        }

        graphics_queue_family_index = static_cast<uint32_t>(std::distance(queue_family_properites.begin(), is_found_graphics_queue_family));
        assert(graphics_queue_family_index < static_cast<uint32_t>(queue_family_properites.size()));

        VkBool32 is_surface_supported = physical_device.getSurfaceSupportKHR(graphics_queue_family_index, surface);
        if (is_surface_supported == VK_FALSE) {
            return false;
        }

        auto surface_formats = physical_device.getSurfaceFormatsKHR(surface);
        auto is_found_surface_format = std::find_if(surface_formats.begin(), surface_formats.end(), []( vk::SurfaceFormatKHR const & surface_format) {
            return (surface_format.format == vk::Format::eR8G8B8A8Snorm || surface_format.format == vk::Format::eB8G8R8A8Unorm)
                && surface_format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
        });
        if (is_found_surface_format == surface_formats.end()) {
            return false;
        }
        surface_format = *is_found_surface_format;

        auto present_modes = physical_device.getSurfacePresentModesKHR(surface);
        auto is_found_present_mode = std::find_if(present_modes.begin(), present_modes.end(), [](vk::PresentModeKHR const& present_mode) {
            return present_mode == vk::PresentModeKHR::eMailbox;
        });
        if (is_found_present_mode == present_modes.end()) {
            return false;
        }
        present_mode = *is_found_present_mode;

        auto physical_device_supproted_extensions = physical_device.enumerateDeviceExtensionProperties();
        auto is_supported = std::find_if(physical_device_supproted_extensions.begin(), physical_device_supproted_extensions.end(), [](vk::ExtensionProperties const& extension_property) {
            return strcmp(extension_property.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0;
        });
        if (is_supported == physical_device_supproted_extensions.end()) {
            return false;
        }
    });
    if (is_found == physical_devices.end()) {
        throw std::runtime_error("Failed to find suitable Device.");
    }

    vk::PhysicalDevice physical_device = *is_found;

    const char* enabled_device_extensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    const char* enabled_device_layers = { "VK_LAYER_KHRONOS_validation" };

    vk::DeviceQueueCreateInfo queue_info({}, graphics_queue_family_index, 1);
    vk::DeviceCreateInfo device_info({}, queue_info, enabled_device_layers, enabled_device_extensions);
    device = physical_device.createDevice(device_info);
}
