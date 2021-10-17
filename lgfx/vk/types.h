// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

class VKFence;
class VKCommandList;
class VKResource;
class VKDescriptorSetLayout;
class VKView;
class VKRenderPass;
class VKFrameBuffer;
class VKDevice;

std::string result_to_string(const vk::Result result) {

	switch(result) {
		case vk::Result::eSuccess: return "Command successfully completed";
		case vk::Result::eNotReady: return "A fence or query has not yet completed";
		case vk::Result::eTimeout: return "A wait operation has not completed in the specified time";
		case vk::Result::eEventSet: return "An event is signaled";
		case vk::Result::eEventReset: return "An event is unsignaled";
		case vk::Result::eIncomplete: return "A return array was too small for the result";
		case vk::Result::eErrorOutOfHostMemory: return "A host memory allocation has failed";
		case vk::Result::eErrorOutOfDeviceMemory: return "A device memory allocation has failed";
		case vk::Result::eErrorInitializationFailed: return "Initialization of an object could not be completed for implementation-specific reasons";
		case vk::Result::eErrorDeviceLost: return "The logical or physical device has been lost";
    	case vk::Result::eErrorMemoryMapFailed: return "Mapping of a memory object has failed";
    	case vk::Result::eErrorLayerNotPresent: return "A requested layer is not present or could not be loaded";
    	case vk::Result::eErrorExtensionNotPresent: return "A requested extension is not supported";
    	case vk::Result::eErrorFeatureNotPresent: return "A requested feature is not supported";
    	case vk::Result::eErrorIncompatibleDriver: return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons";
    	case vk::Result::eErrorTooManyObjects: return "Too many objects of the type have already been created";
    	case vk::Result::eErrorFormatNotSupported: return "A requested format is not supported on this device";
    	case vk::Result::eErrorFragmentedPool: return "A pool allocation has failed due to fragmentation of the pool’s memory";
    	default: return "An unknown error has occurred";
	}
}

extern constexpr char* api_name = "Vulkan";

}

#ifndef THROW_IF_FAILED
#define THROW_IF_FAILED(VKResult) if(VKResult != vk::Result::eSuccess) throw std::runtime_error(result_to_string(VKResult));
#endif