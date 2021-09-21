// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::wrapper {

DXGI_FORMAT convert_format(const Format format) {

	switch(format) {
		case Format::Unknown: return DXGI_FORMAT_UNKNOWN;
		case Format::RGBA32float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Format::RGBA32uint: return DXGI_FORMAT_R32G32B32A32_UINT;
		case Format::RGBA32int: return DXGI_FORMAT_R32G32B32A32_SINT;
		case Format::RGB32float: return DXGI_FORMAT_R32G32B32_FLOAT;
		case Format::RGB32uint: return DXGI_FORMAT_R32G32B32_UINT;
		case Format::RGB32int: return DXGI_FORMAT_R32G32B32_SINT;
		case Format::RG32float: return DXGI_FORMAT_R32G32_FLOAT;
		case Format::RG32uint: return DXGI_FORMAT_R32G32_UINT;
		case Format::RG32int: return DXGI_FORMAT_R32G32_SINT;
		case Format::R32float: return DXGI_FORMAT_R32_FLOAT;
		case Format::R32uint: return DXGI_FORMAT_R32_UINT;
		case Format::R32int: return DXGI_FORMAT_R32_SINT;
		case Format::RGBA16float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case Format::RGBA16uint: return DXGI_FORMAT_R16G16B16A16_UINT;
		case Format::RGBA16int: return DXGI_FORMAT_R16G16B16A16_SINT;
		case Format::RGBA16unorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
		case Format::RGBA16snorm: return DXGI_FORMAT_R16G16B16A16_SNORM;
		case Format::RG16float: return DXGI_FORMAT_R16G16_FLOAT;
		case Format::RG16uint: return DXGI_FORMAT_R16G16_UINT;
		case Format::RG16int: return DXGI_FORMAT_R16G16_SINT;
		case Format::RG16unorm: return DXGI_FORMAT_R16G16_UNORM;
		case Format::RG16snorm: return DXGI_FORMAT_R16G16_SNORM;
		case Format::R16float: return DXGI_FORMAT_R16_FLOAT;
		case Format::R16uint: return DXGI_FORMAT_R16_UINT;
		case Format::R16int: return DXGI_FORMAT_R16_SINT;
		case Format::R16unorm: return DXGI_FORMAT_R16_UNORM;
		case Format::R16snorm: return DXGI_FORMAT_R16_SNORM;
		case Format::RGBA8uint: return DXGI_FORMAT_R8G8B8A8_UINT;
		case Format::RGBA8int: return DXGI_FORMAT_R8G8B8A8_SINT;
		case Format::RGBA8unorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Format::RGBA8snorm: return DXGI_FORMAT_R8G8B8A8_SNORM;
		case Format::RG8uint: return DXGI_FORMAT_R8G8_UINT;
		case Format::RG8int: return DXGI_FORMAT_R8G8_SINT;
		case Format::RG8unorm: return DXGI_FORMAT_R8G8_UNORM;
		case Format::RG8snorm: return DXGI_FORMAT_R8G8_SNORM;
		case Format::R8uint: return DXGI_FORMAT_R8_UINT;
		case Format::R8int: return DXGI_FORMAT_R8_SINT;
		case Format::R8unorm: return DXGI_FORMAT_R8_UNORM;
		case Format::R8snorm: return DXGI_FORMAT_R8_SNORM;
		default: assert(false && "passed invalid argument to convert_format"); return DXGI_FORMAT_UNKNOWN;
	}
}

D3D12_FILL_MODE convert_fill_mode(const FillMode fill_mode) {
	
	switch(fill_mode) {
		case FillMode::Solid: return D3D12_FILL_MODE_SOLID;
		case FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
		default: assert(false && "passed invalid argument to convert_fill_mode"); return D3D12_FILL_MODE_SOLID;
	}
}

D3D12_CULL_MODE convert_cull_mode(const CullMode cull_mode) {
	
	switch(cull_mode) {
		case CullMode::None: return D3D12_CULL_MODE_NONE;
		case CullMode::Back: return D3D12_CULL_MODE_BACK;
		case CullMode::Front: return D3D12_CULL_MODE_FRONT;
		default: assert(false && "passed invalid argument to convert_cull_mode"); return D3D12_CULL_MODE_NONE;
	}
}

D3D12_COMPARISON_FUNC convert_comparison_func(const ComparisonFunc comparison_func) {
	
	switch(comparison_func) {
		case ComparisonFunc::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
		case ComparisonFunc::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
		case ComparisonFunc::Greater: return D3D12_COMPARISON_FUNC_GREATER;
		case ComparisonFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case ComparisonFunc::Less: return D3D12_COMPARISON_FUNC_LESS;
		case ComparisonFunc::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case ComparisonFunc::Never: return D3D12_COMPARISON_FUNC_NEVER;
		case ComparisonFunc::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		default: assert(false && "passed invalid argument to convert_comparison_func"); return D3D12_COMPARISON_FUNC_ALWAYS;
	}
}

D3D12_STENCIL_OP convert_stencil_op(const StencilOp stencil_op) {
	
	switch(stencil_op) {
		case StencilOp::Zero: return D3D12_STENCIL_OP_ZERO;
		case StencilOp::Replace: return D3D12_STENCIL_OP_REPLACE;
		case StencilOp::Keep: return D3D12_STENCIL_OP_KEEP;
		case StencilOp::Invert: return D3D12_STENCIL_OP_INVERT;
		case StencilOp::IncrSat: return D3D12_STENCIL_OP_INCR_SAT;
		case StencilOp::Incr: return D3D12_STENCIL_OP_INCR;
		case StencilOp::DecrSat: return D3D12_STENCIL_OP_DECR_SAT;
		case StencilOp::Decr: return D3D12_STENCIL_OP_DECR;
		default: assert(false && "passed invalid argument to convert_stencil_op"); return D3D12_STENCIL_OP_ZERO;
	}
}

D3D12_BLEND convert_blend(const Blend blend) {
	
	switch(blend) {
		case Blend::Zero: return D3D12_BLEND_ZERO;
		case Blend::One: return D3D12_BLEND_ONE;
		case Blend::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
		case Blend::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
		default: assert(false && "passed invalid argument to convert_blend"); return D3D12_BLEND_ZERO;
	}
}

D3D12_BLEND_OP convert_blend_op(const BlendOp blend_op) {
	
	switch(blend_op) {
		case BlendOp::Add: return D3D12_BLEND_OP_ADD;
		case BlendOp::Min: return D3D12_BLEND_OP_MIN;
		case BlendOp::Max: return D3D12_BLEND_OP_MAX;
		case BlendOp::RevSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
		case BlendOp::Subtract: return D3D12_BLEND_OP_SUBTRACT;
		default: assert(false && "passed invalid argument to convert_blend_op"); return static_cast<D3D12_BLEND_OP>(0);
	}
}

D3D12_SHADER_VISIBILITY convert_shader_visibility(const ShaderType shader_type) {
	
	switch(shader_type) {
		case ShaderType::Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
		case ShaderType::Pixel: return D3D12_SHADER_VISIBILITY_PIXEL;
		case ShaderType::Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
		case ShaderType::Hull: return D3D12_SHADER_VISIBILITY_HULL;
		case ShaderType::Domain: return D3D12_SHADER_VISIBILITY_DOMAIN;
		case ShaderType::All: return D3D12_SHADER_VISIBILITY_ALL;
		case ShaderType::Compute: return D3D12_SHADER_VISIBILITY_ALL;
		default: assert(false && "passed invalid argument to convert_shader_visibility"); return D3D12_SHADER_VISIBILITY_ALL;
	}
}

D3D12_DESCRIPTOR_RANGE_TYPE convert_descriptor_range_type(const ViewType view_type) {
	
	switch(view_type) {
		case ViewType::Texture:
		case ViewType::StructuredBuffer:
		case ViewType::Buffer: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		case ViewType::RWTexture:
		case ViewType::RWBuffer:
		case ViewType::RWStructuredBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		case ViewType::ConstantBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		case ViewType::Sampler: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		default: assert(false && "passed invalid argument to convert_descriptor_range_type"); return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	}
}

D3D12_COMMAND_LIST_TYPE convert_command_list_type(const CommandListType cmd_list_type) {
	
	switch(cmd_list_type) {
        case CommandListType::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case CommandListType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
        case CommandListType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		default: assert(false && "passed invalid argument to convert_command_list_type"); return D3D12_COMMAND_LIST_TYPE_DIRECT;
    }
}

D3D12_HEAP_TYPE convert_heap_type(const MemoryType memory_type) {
	
	switch(memory_type) {
        case MemoryType::Default: return D3D12_HEAP_TYPE_DEFAULT;
        case MemoryType::Upload: return D3D12_HEAP_TYPE_UPLOAD;
        case MemoryType::Readback: return D3D12_HEAP_TYPE_READBACK;
		default: assert(false && "passed invalid argument to convert_heap_type"); return D3D12_HEAP_TYPE_DEFAULT;
    }
}

D3D12_RESOURCE_STATES convert_resource_state(const ResourceState resource_state) {
	
	switch(resource_state) {
		case ResourceState::Common: return D3D12_RESOURCE_STATE_COMMON;
		case ResourceState::VertexAndConstantBuffer: return D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
		case ResourceState::IndexBuffer: return D3D12_RESOURCE_STATE_INDEX_BUFFER;
		case ResourceState::RenderTarget: return D3D12_RESOURCE_STATE_RENDER_TARGET;
		case ResourceState::UnorderedAccess: return D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
		case ResourceState::DepthStencilWrite: return D3D12_RESOURCE_STATE_DEPTH_WRITE;
		case ResourceState::DepthStencilRead: return D3D12_RESOURCE_STATE_DEPTH_READ;
		case ResourceState::NonPixelShaderResource: return D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE;
		case ResourceState::PixelShaderResource: return D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
		case ResourceState::CopyDest: return D3D12_RESOURCE_STATE_COPY_DEST;
		case ResourceState::CopySource: return D3D12_RESOURCE_STATE_COPY_SOURCE;
		case ResourceState::Present: return D3D12_RESOURCE_STATE_PRESENT;
		case ResourceState::GenericRead: return D3D12_RESOURCE_STATE_GENERIC_READ;
		default: assert(false && "passed invalid argument to convert_resource_state"); return D3D12_RESOURCE_STATE_COMMON;
	}
}

D3D12_RESOURCE_DIMENSION convert_resource_dimension(const ViewDimension view_dimension) {
	
	switch(view_dimension) {
		case ViewDimension::Buffer: return D3D12_RESOURCE_DIMENSION_BUFFER;
		case ViewDimension::Texture1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		case ViewDimension::Texture2D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case ViewDimension::Texture3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		default: assert(false && "passed invalid argument to convert_resource_dimension"); return D3D12_RESOURCE_DIMENSION_UNKNOWN;
	}
}

D3D12_FILTER convert_filter(const Filter filter) {
	
	switch(filter) {
		case Filter::Anisotropic: return D3D12_FILTER_ANISOTROPIC;
		case Filter::ComparisonMinMagMipLinear: return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		case Filter::MinMagMipLinear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		default: assert(false && "passed invalid argument to convert_filter"); return D3D12_FILTER_ANISOTROPIC;
	}
}

D3D12_TEXTURE_ADDRESS_MODE convert_texture_address_mode(const TextureAddressMode address_mode) {
	
	switch(address_mode) {
		case TextureAddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case TextureAddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		default: assert(false && "passed invalid argument to convert_texture_address_mode"); return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	}
}

D3D12_DESCRIPTOR_HEAP_TYPE convert_descriptor_heap_type(const ViewType view_type) {
	
	switch(view_type) {
		case ViewType::Texture:
		case ViewType::StructuredBuffer:
		case ViewType::Buffer: 
		case ViewType::RWTexture:
		case ViewType::RWBuffer:
		case ViewType::RWStructuredBuffer:
		case ViewType::ConstantBuffer: return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		case ViewType::Sampler: return D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
		case ViewType::RenderTarget: return D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		case ViewType::DepthStencil: return D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		default: assert(false && "passed invalid argument to convert_descriptor_heap_type"); return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	}
}

D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE convert_render_pass_type(const RenderPassLoadOp load_op) {
	
	switch (load_op) {
    	case RenderPassLoadOp::Load: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
    	case RenderPassLoadOp::Clear: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		case RenderPassLoadOp::DontCare: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
		default: assert(false && "passed invalid argument to convert_render_pass_type"); return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
    }
}

D3D12_RENDER_PASS_ENDING_ACCESS_TYPE convert_render_pass_type(const RenderPassStoreOp store_op) {
    
	switch (store_op) {
    	case RenderPassStoreOp::Store: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
    	case RenderPassStoreOp::DontCare: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
		default: assert(false && "passed invalid argument to convert_render_pass_type"); return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
    }
}

std::string result_to_string(const HRESULT result) {
	switch(result) {
		case E_FAIL: return "Attempted to create a device with the debug layer enabled and the layer is not installed";
		case E_INVALIDARG: return "An invalid parameter was passed to the returning function";
		case E_OUTOFMEMORY: return "Direct3D could not allocate sufficient memory to complete the call";
		case E_NOTIMPL: return "The method call isn't implemented with the passed parameter combination";
		case S_FALSE: return "Alternate success value, indicating a successful but nonstandard completion";
		case S_OK: return "No error occurred";
		case D3D12_ERROR_ADAPTER_NOT_FOUND: return "The specified cached PSO was created on a different adapter and cannot be reused on the current adapter";
		case D3D12_ERROR_DRIVER_VERSION_MISMATCH: return "The specified cached PSO was created on a different driver version and cannot be reused on the current adapter";
		case DXGI_ERROR_INVALID_CALL: return "The method call is invalid. For example, a method's parameter may not be a valid pointer";
		case DXGI_ERROR_WAS_STILL_DRAWING: return "The previous blit operation that is transferring information to or from this surface is incomplete";
		default: return "An unknown error has occurred";
	}
}

}

#ifndef ASSERT_SUCCEEDED
#define ASSERT_SUCCEEDED(result) if(FAILED(result)) { assert(false); }
#endif