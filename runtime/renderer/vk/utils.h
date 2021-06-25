// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer {

std::string result_to_string(const VkResult result) {
	switch(result) {
		case VK_SUCCESS: return "Command successfully completed";
		case VK_NOT_READY: return "A fence or query has not yet completed";
		case VK_TIMEOUT: return "A wait operation has not completed in the specified time";
		case VK_EVENT_SET: return "An event is signaled";
		case VK_EVENT_RESET: return "An event is unsignaled";
		case VK_INCOMPLETE: return "A return array was too small for the result";
		case VK_ERROR_OUT_OF_HOST_MEMORY: return "A host memory allocation has failed";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "A device memory allocation has failed";
		case VK_ERROR_INITIALIZATION_FAILED: return "Initialization of an object could not be completed for implementation-specific reasons";
		case VK_ERROR_DEVICE_LOST: return "The logical or physical device has been lost";
    	case VK_ERROR_MEMORY_MAP_FAILED: return "Mapping of a memory object has failed";
    	case VK_ERROR_LAYER_NOT_PRESENT: return "A requested layer is not present or could not be loaded";
    	case VK_ERROR_EXTENSION_NOT_PRESENT: return "A requested extension is not supported";
    	case VK_ERROR_FEATURE_NOT_PRESENT: return "A requested feature is not supported";
    	case VK_ERROR_INCOMPATIBLE_DRIVER: return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons";
    	case VK_ERROR_TOO_MANY_OBJECTS: return "Too many objects of the type have already been created";
    	case VK_ERROR_FORMAT_NOT_SUPPORTED: return "A requested format is not supported on this device";
    	case VK_ERROR_FRAGMENTED_POOL: return "A pool allocation has failed due to fragmentation of the pool’s memory";
    	case VK_ERROR_UNKNOWN: return "An unknown error has occurred";
		default: return "An unknown error has occurred"; 
	}
}

void throw_if_failed(const VkResult result) {
	if (result != VK_SUCCESS) {
		throw std::runtime_error(result_to_string(result));
	}
}

}