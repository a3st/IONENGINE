// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::gfx {

DXGI_FORMAT dxgi_format_to_gfx_enum(const Format format) {

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
		default: assert(false && "passed invalid argument to dxgi_format_to_gfx_enum"); return DXGI_FORMAT_UNKNOWN;
	}
}

D3D12_FILL_MODE d3d12_fill_mode_to_gfx_enum(const FillMode fill_mode) {
	
	switch(fill_mode) {
		case FillMode::Solid: return D3D12_FILL_MODE_SOLID;
		case FillMode::Wireframe: return D3D12_FILL_MODE_WIREFRAME;
		default: assert(false && "passed invalid argument to d3d12_fill_mode_to_gfx_enum"); return D3D12_FILL_MODE_SOLID;
	}
}

D3D12_CULL_MODE d3d12_cull_mode_to_gfx_enum(const CullMode cull_mode) {
	
	switch(cull_mode) {
		case CullMode::None: return D3D12_CULL_MODE_NONE;
		case CullMode::Back: return D3D12_CULL_MODE_BACK;
		case CullMode::Front: return D3D12_CULL_MODE_FRONT;
		default: assert(false && "passed invalid argument to d3d12_cull_mode_to_gfx_enum"); return D3D12_CULL_MODE_NONE;
	}
}

D3D12_COMPARISON_FUNC d3d12_comparison_func_to_gfx_enum(const ComparisonFunc comparison_func) {
	
	switch(comparison_func) {
		case ComparisonFunc::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
		case ComparisonFunc::Equal: return D3D12_COMPARISON_FUNC_EQUAL;
		case ComparisonFunc::Greater: return D3D12_COMPARISON_FUNC_GREATER;
		case ComparisonFunc::GreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case ComparisonFunc::Less: return D3D12_COMPARISON_FUNC_LESS;
		case ComparisonFunc::LessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case ComparisonFunc::Never: return D3D12_COMPARISON_FUNC_NEVER;
		case ComparisonFunc::NotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		default: assert(false && "passed invalid argument to d3d12_comparison_func_to_gfx_enum"); return D3D12_COMPARISON_FUNC_ALWAYS;
	}
}

D3D12_STENCIL_OP d3d12_stencil_op_to_gfx_enum(const StencilOp stencil_op) {
	
	switch(stencil_op) {
		case StencilOp::Zero: return D3D12_STENCIL_OP_ZERO;
		case StencilOp::Replace: return D3D12_STENCIL_OP_REPLACE;
		case StencilOp::Keep: return D3D12_STENCIL_OP_KEEP;
		case StencilOp::Invert: return D3D12_STENCIL_OP_INVERT;
		case StencilOp::IncrSat: return D3D12_STENCIL_OP_INCR_SAT;
		case StencilOp::Incr: return D3D12_STENCIL_OP_INCR;
		case StencilOp::DecrSat: return D3D12_STENCIL_OP_DECR_SAT;
		case StencilOp::Decr: return D3D12_STENCIL_OP_DECR;
		default: assert(false && "passed invalid argument to d3d12_stencil_op_to_gfx_enum"); return D3D12_STENCIL_OP_ZERO;
	}
}

D3D12_BLEND d3d12_blend_to_gfx_enum(const Blend blend) {
	
	switch(blend) {
		case Blend::Zero: return D3D12_BLEND_ZERO;
		case Blend::One: return D3D12_BLEND_ONE;
		case Blend::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
		case Blend::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
		default: assert(false && "passed invalid argument to d3d12_blend_to_gfx_enum"); return D3D12_BLEND_ZERO;
	}
}

D3D12_BLEND_OP d3d12_blend_op_to_gfx_enum(const BlendOp blend_op) {
	
	switch(blend_op) {
		case BlendOp::Add: return D3D12_BLEND_OP_ADD;
		case BlendOp::Min: return D3D12_BLEND_OP_MIN;
		case BlendOp::Max: return D3D12_BLEND_OP_MAX;
		case BlendOp::RevSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
		case BlendOp::Subtract: return D3D12_BLEND_OP_SUBTRACT;
		default: assert(false && "passed invalid argument to d3d12_blend_op_to_gfx_enum"); return static_cast<D3D12_BLEND_OP>(0);
	}
}

D3D12_SHADER_VISIBILITY d3d12_shader_visibility_to_gfx_enum(const ShaderType shader_type) {
	
	switch(shader_type) {
		case ShaderType::Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
		case ShaderType::Pixel: return D3D12_SHADER_VISIBILITY_PIXEL;
		case ShaderType::Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
		case ShaderType::Hull: return D3D12_SHADER_VISIBILITY_HULL;
		case ShaderType::Domain: return D3D12_SHADER_VISIBILITY_DOMAIN;
		case ShaderType::All: return D3D12_SHADER_VISIBILITY_ALL;
		case ShaderType::Compute: return D3D12_SHADER_VISIBILITY_ALL;
		default: assert(false && "passed invalid argument to d3d12_shader_visibility_to_gfx_enum"); return D3D12_SHADER_VISIBILITY_ALL;
	}
}

D3D12_DESCRIPTOR_RANGE_TYPE d3d12_descriptor_range_type_to_gfx_enum(const ViewType view_type) {
	
	switch(view_type) {
		case ViewType::Texture:
		case ViewType::StructuredBuffer:
		case ViewType::Buffer: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		case ViewType::RWTexture:
		case ViewType::RWBuffer:
		case ViewType::RWStructuredBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		case ViewType::ConstantBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		case ViewType::Sampler: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		default: assert(false && "passed invalid argument to d3d12_descriptor_range_type_to_gfx_enum"); return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	}
}

D3D12_COMMAND_LIST_TYPE d3d12_command_list_type_to_gfx_enum(const CommandListType command_list_type) {
	
	switch(command_list_type) {
        case CommandListType::Graphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case CommandListType::Copy: return D3D12_COMMAND_LIST_TYPE_COPY;
        case CommandListType::Compute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		default: assert(false && "passed invalid argument to d3d12_command_list_type_to_gfx_enum"); return D3D12_COMMAND_LIST_TYPE_DIRECT;
    }
}

D3D12_HEAP_TYPE d3d12_heap_type_to_gfx_enum(const MemoryType memory_type) {
	
	switch(memory_type) {
        case MemoryType::Default: return D3D12_HEAP_TYPE_DEFAULT;
        case MemoryType::Upload: return D3D12_HEAP_TYPE_UPLOAD;
        case MemoryType::Readback: return D3D12_HEAP_TYPE_READBACK;
		default: assert(false && "passed invalid argument to d3d12_heap_type_to_gfx_enum"); return D3D12_HEAP_TYPE_DEFAULT;
    }
}

D3D12_RESOURCE_STATES d3d12_resource_state_to_gfx_enum(const ResourceState resource_state) {
	
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
		default: assert(false && "passed invalid argument to d3d12_resource_states_to_gfx_enum"); return D3D12_RESOURCE_STATE_COMMON;
	}
}

D3D12_RESOURCE_DIMENSION d3d12_resource_dimension_to_gfx_enum(const ViewDimension view_dimension) {
	
	switch(view_dimension) {
		case ViewDimension::Buffer: return D3D12_RESOURCE_DIMENSION_BUFFER;
		case ViewDimension::Texture1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		case ViewDimension::Texture2D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case ViewDimension::Texture3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		default: assert(false && "passed invalid argument to d3d12_resource_dimension_to_gfx_enum"); return D3D12_RESOURCE_DIMENSION_UNKNOWN;
	}
}

D3D12_FILTER d3d12_filter_to_gfx_enum(const Filter filter) {
	
	switch(filter) {
		case Filter::Anisotropic: return D3D12_FILTER_ANISOTROPIC;
		case Filter::ComparisonMinMagMipLinear: return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		case Filter::MinMagMipLinear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		default: assert(false && "passed invalid argument to d3d12_filter_to_gfx_enum"); return D3D12_FILTER_ANISOTROPIC;
	}
}

D3D12_TEXTURE_ADDRESS_MODE d3d12_texture_address_mode_to_gfx_enum(const TextureAddressMode address_mode) {
	
	switch(address_mode) {
		case TextureAddressMode::Clamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case TextureAddressMode::Wrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		default: assert(false && "passed invalid argument to d3d12_texture_address_mode_to_gfx_enum"); return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	}
}

D3D12_DESCRIPTOR_HEAP_TYPE d3d12_descriptor_heap_type_to_gfx_enum(const ViewType view_type) {
	
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
		default: assert(false && "passed invalid argument to d3d12_descriptor_heap_type_to_gfx_enum"); return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	}
}

D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE d3d12_render_pass_begin_type_to_gfx_enum(const RenderPassLoadOp load_op) {
	
	switch (load_op) {
    	case RenderPassLoadOp::Load: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
    	case RenderPassLoadOp::Clear: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		case RenderPassLoadOp::DontCare: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
		default: assert(false && "passed invalid argument to d3d12_render_pass_begin_type_to_gfx_enum"); return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
    }
}

D3D12_RENDER_PASS_ENDING_ACCESS_TYPE d3d12_render_pass_end_type_to_gfx_enum(const RenderPassStoreOp store_op) {
    
	switch (store_op) {
    	case RenderPassStoreOp::Store: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
    	case RenderPassStoreOp::DontCare: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
		default: assert(false && "passed invalid argument to d3d12_render_pass_end_type_to_gfx_enum"); return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
    }
}

}