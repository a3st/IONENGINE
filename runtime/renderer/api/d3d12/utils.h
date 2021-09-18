// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::renderer::api {

D3D12_FILL_MODE convert_fill_mode(const FillMode fill_mode) {
	switch(fill_mode) {
		case FillMode::Solid: return D3D12_FILL_MODE_SOLID;
		case FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
		default: assert(false && "passed invalid argument to convert_fill_mode"); return static_cast<D3D12_FILL_MODE>(0);
	}
}

D3D12_CULL_MODE convert_cull_mode(const CullMode cull_mode) {
	switch(cull_mode) {
		case CullMode::None: return D3D12_CULL_MODE_NONE;
		case CullMode::Back: return D3D12_CULL_MODE_BACK;
		case CullMode::Front: return D3D12_CULL_MODE_FRONT;
		default: assert(false && "passed invalid argument to convert_cull_mode"); return static_cast<D3D12_CULL_MODE>(0);
	}
}

D3D12_COMPARISON_FUNC convert_comparison_func(const ComparisonFunc func) {
	switch(func) {
		case ComparisonFunc::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
		case ComparisonFunc::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
		case ComparisonFunc::Greater: return D3D12_COMPARISON_FUNC_GREATER;
		case ComparisonFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case ComparisonFunc::Less: return D3D12_COMPARISON_FUNC_LESS;
		case ComparisonFunc::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case ComparisonFunc::Never: return D3D12_COMPARISON_FUNC_NEVER;
		case ComparisonFunc::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		default: assert(false && "passed invalid argument to convert_comparison_func"); return static_cast<D3D12_COMPARISON_FUNC>(0);
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
		default: assert(false && "passed invalid argument to convert_stencil_op"); return static_cast<D3D12_STENCIL_OP>(0);
	}
}

D3D12_BLEND convert_blend(const Blend blend) {
	switch(blend) {
		case Blend::Zero: return D3D12_BLEND_ZERO;
		case Blend::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
		case Blend::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
		default: assert(false && "passed invalid argument to convert_blend"); return static_cast<D3D12_BLEND>(0);
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
		default: assert(false && "passed invalid argument to convert_shader_visibility"); return static_cast<D3D12_SHADER_VISIBILITY>(0);
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
		default: assert(false && "passed invalid argument to convert_descriptor_range_type"); return static_cast<D3D12_DESCRIPTOR_RANGE_TYPE>(0);
	}
}

D3D12_COMMAND_LIST_TYPE convert_command_list_type(const CommandListType list_type) {
	switch(list_type) {
        case CommandListType::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case CommandListType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
        case CommandListType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		default: assert(false && "passed invalid argument to convert_command_list_type"); return static_cast<D3D12_COMMAND_LIST_TYPE>(0);
    }
}

D3D12_HEAP_TYPE convert_heap_type(const MemoryType heap_type) {
	switch(heap_type) {
        case MemoryType::Default: return D3D12_HEAP_TYPE_DEFAULT;
        case MemoryType::Upload: return D3D12_HEAP_TYPE_UPLOAD;
        case MemoryType::Readback: return D3D12_HEAP_TYPE_READBACK;
		default: assert(false && "passed invalid argument to convert_heap_type"); return static_cast<D3D12_HEAP_TYPE>(0);
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
		default: assert(false && "passed invalid argument to convert_resource_state"); return static_cast<D3D12_RESOURCE_STATES>(0);
	}
}

D3D12_RESOURCE_DIMENSION convert_resource_dimension(const ViewDimension dimension) {
	switch(dimension) {
		case ViewDimension::Buffer: return D3D12_RESOURCE_DIMENSION_BUFFER;
		case ViewDimension::Texture1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		case ViewDimension::Texture2D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case ViewDimension::Texture3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		default: assert(false && "passed invalid argument to convert_resource_dimension"); return static_cast<D3D12_RESOURCE_DIMENSION>(0);
	}
}

D3D12_FILTER convert_filter(const Filter filter) {
	switch(filter) {
		case Filter::Anisotropic: return D3D12_FILTER_ANISOTROPIC;
		case Filter::ComparisonMinMagMipLinear: return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		case Filter::MinMagMipLinear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		default: assert(false && "passed invalid argument to convert_filter"); return static_cast<D3D12_FILTER>(0);
	}
}

D3D12_TEXTURE_ADDRESS_MODE convert_texture_address_mode(const TextureAddressMode address_mode) {
	switch(address_mode) {
		case TextureAddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case TextureAddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		default: assert(false && "passed invalid argument to convert_texture_address_mode"); return static_cast<D3D12_TEXTURE_ADDRESS_MODE>(0);
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
		default: assert(false && "passed invalid argument to convert_descriptor_heap_type"); return static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(0);
	}
}

D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE convert_render_pass_type(const RenderPassLoadOp load_op) {
	switch (load_op) {
    	case RenderPassLoadOp::Load: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
    	case RenderPassLoadOp::Clear: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		case RenderPassLoadOp::DontCare: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
		default: assert(false && "passed invalid argument to convert_render_pass_type"); return static_cast<D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE>(0);
    }
}

D3D12_RENDER_PASS_ENDING_ACCESS_TYPE convert_render_pass_type(const RenderPassStoreOp store_op) {
    switch (store_op) {
    	case RenderPassStoreOp::Store: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
    	case RenderPassStoreOp::DontCare: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
		default: assert(false && "passed invalid argument to convert_render_pass_type"); return static_cast<D3D12_RENDER_PASS_ENDING_ACCESS_TYPE>(0);
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