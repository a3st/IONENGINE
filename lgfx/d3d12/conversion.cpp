// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#include "../precompiled.h"
#include "conversion.h"

namespace lgfx {

DXGI_FORMAT GFXToDXGIFormat(const Format format) {

	switch(format) {
		case Format::kUnknown: return DXGI_FORMAT_UNKNOWN;
		case Format::kRGBA32float: return DXGI_FORMAT_R32G32B32A32_FLOAT;
		case Format::kRGBA32uint: return DXGI_FORMAT_R32G32B32A32_UINT;
		case Format::kRGBA32int: return DXGI_FORMAT_R32G32B32A32_SINT;
		case Format::kRGB32float: return DXGI_FORMAT_R32G32B32_FLOAT;
		case Format::kRGB32uint: return DXGI_FORMAT_R32G32B32_UINT;
		case Format::kRGB32int: return DXGI_FORMAT_R32G32B32_SINT;
		case Format::kRG32float: return DXGI_FORMAT_R32G32_FLOAT;
		case Format::kRG32uint: return DXGI_FORMAT_R32G32_UINT;
		case Format::kRG32int: return DXGI_FORMAT_R32G32_SINT;
		case Format::kR32float: return DXGI_FORMAT_R32_FLOAT;
		case Format::kR32uint: return DXGI_FORMAT_R32_UINT;
		case Format::kR32int: return DXGI_FORMAT_R32_SINT;
		case Format::kRGBA16float: return DXGI_FORMAT_R16G16B16A16_FLOAT;
		case Format::kRGBA16uint: return DXGI_FORMAT_R16G16B16A16_UINT;
		case Format::kRGBA16int: return DXGI_FORMAT_R16G16B16A16_SINT;
		case Format::kRGBA16unorm: return DXGI_FORMAT_R16G16B16A16_UNORM;
		case Format::kRGBA16snorm: return DXGI_FORMAT_R16G16B16A16_SNORM;
		case Format::kRG16float: return DXGI_FORMAT_R16G16_FLOAT;
		case Format::kRG16uint: return DXGI_FORMAT_R16G16_UINT;
		case Format::kRG16int: return DXGI_FORMAT_R16G16_SINT;
		case Format::kRG16unorm: return DXGI_FORMAT_R16G16_UNORM;
		case Format::kRG16snorm: return DXGI_FORMAT_R16G16_SNORM;
		case Format::kR16float: return DXGI_FORMAT_R16_FLOAT;
		case Format::kR16uint: return DXGI_FORMAT_R16_UINT;
		case Format::kR16int: return DXGI_FORMAT_R16_SINT;
		case Format::kR16unorm: return DXGI_FORMAT_R16_UNORM;
		case Format::kR16snorm: return DXGI_FORMAT_R16_SNORM;
		case Format::kRGBA8uint: return DXGI_FORMAT_R8G8B8A8_UINT;
		case Format::kRGBA8int: return DXGI_FORMAT_R8G8B8A8_SINT;
		case Format::kRGBA8unorm: return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Format::kRGBA8snorm: return DXGI_FORMAT_R8G8B8A8_SNORM;
		case Format::kRG8uint: return DXGI_FORMAT_R8G8_UINT;
		case Format::kRG8int: return DXGI_FORMAT_R8G8_SINT;
		case Format::kRG8unorm: return DXGI_FORMAT_R8G8_UNORM;
		case Format::kRG8snorm: return DXGI_FORMAT_R8G8_SNORM;
		case Format::kR8uint: return DXGI_FORMAT_R8_UINT;
		case Format::kR8int: return DXGI_FORMAT_R8_SINT;
		case Format::kR8unorm: return DXGI_FORMAT_R8_UNORM;
		case Format::kR8snorm: return DXGI_FORMAT_R8_SNORM;
		default: assert(false && "passed invalid argument to GFXToDXGIFormat"); return DXGI_FORMAT_UNKNOWN;
	}
}

Format DXGIFormatToGFX(const DXGI_FORMAT format) {

	switch(format) {
		case DXGI_FORMAT_UNKNOWN: return Format::kUnknown;
		case DXGI_FORMAT_R32G32B32A32_FLOAT: return Format::kRGBA32float;
		case DXGI_FORMAT_R32G32B32A32_UINT: return Format::kRGBA32uint;
		case DXGI_FORMAT_R32G32B32A32_SINT: return Format::kRGBA32int;
		case DXGI_FORMAT_R32G32B32_FLOAT: return Format::kRGB32float;
		case DXGI_FORMAT_R32G32B32_UINT: return Format::kRGB32uint;
		case DXGI_FORMAT_R32G32B32_SINT: return Format::kRGB32int;
		case DXGI_FORMAT_R32G32_FLOAT: return Format::kRG32float;
		case DXGI_FORMAT_R32G32_UINT: return Format::kRG32uint;
		case DXGI_FORMAT_R32G32_SINT: return Format::kRG32int;
		case DXGI_FORMAT_R32_FLOAT: return Format::kR32float;
		case DXGI_FORMAT_R32_UINT: return Format::kR32uint;
		case DXGI_FORMAT_R32_SINT: return Format::kR32int;
		case DXGI_FORMAT_R16G16B16A16_FLOAT: return Format::kRGBA16float;
		case DXGI_FORMAT_R16G16B16A16_UINT: return Format::kRGBA16uint;
		case DXGI_FORMAT_R16G16B16A16_SINT: return Format::kRGBA16int;
		case DXGI_FORMAT_R16G16B16A16_UNORM: return Format::kRGBA16unorm;
		case DXGI_FORMAT_R16G16B16A16_SNORM: return Format::kRGBA16snorm;
		case DXGI_FORMAT_R16G16_FLOAT: return Format::kRG16float;
		case DXGI_FORMAT_R16G16_UINT: return Format::kRG16uint;
		case DXGI_FORMAT_R16G16_SINT: return Format::kRG16int;
		case DXGI_FORMAT_R16G16_UNORM: return Format::kRG16unorm;
		case DXGI_FORMAT_R16G16_SNORM: return Format::kRG16snorm;
		case DXGI_FORMAT_R16_FLOAT: return Format::kR16float;
		case DXGI_FORMAT_R16_UINT: return Format::kR16uint;
		case DXGI_FORMAT_R16_SINT: return Format::kR16int;
		case DXGI_FORMAT_R16_UNORM: return Format::kR16unorm;
		case DXGI_FORMAT_R16_SNORM: return Format::kR16snorm;
		case DXGI_FORMAT_R8G8B8A8_UINT: return Format::kRGBA8uint;
		case DXGI_FORMAT_R8G8B8A8_SINT: return Format::kRGBA8int;
		case DXGI_FORMAT_R8G8B8A8_UNORM: return Format::kRGBA8unorm;
		case DXGI_FORMAT_R8G8B8A8_SNORM: return Format::kRGBA8snorm;
		case DXGI_FORMAT_R8G8_UINT: return Format::kRG8uint;
		case DXGI_FORMAT_R8G8_SINT: return Format::kRG8int;
		case DXGI_FORMAT_R8G8_UNORM: return Format::kRG8unorm;
		case DXGI_FORMAT_R8G8_SNORM: return Format::kRG8snorm;
		case DXGI_FORMAT_R8_UINT: return Format::kR8uint;
		case DXGI_FORMAT_R8_SINT: return Format::kR8int;
		case DXGI_FORMAT_R8_UNORM: return Format::kR8unorm;
		case DXGI_FORMAT_R8_SNORM: return Format::kR8snorm;
		default: assert(false && "passed invalid argument to DXGIFormatToGFX"); return Format::kUnknown;
	}
}

/*D3D12_FILL_MODE gfx_to_d3d12_fill_mode(const FillMode fill_mode) {
	
	switch(fill_mode) {
		case FillMode::kSolid: return D3D12_FILL_MODE_SOLID;
		case FillMode::kWireframe: return D3D12_FILL_MODE_WIREFRAME;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_fill_mode"); return D3D12_FILL_MODE_SOLID;
	}
}

D3D12_CULL_MODE gfx_to_d3d12_cull_mode(const CullMode cull_mode) {
	
	switch(cull_mode) {
		case CullMode::None: return D3D12_CULL_MODE_NONE;
		case CullMode::Back: return D3D12_CULL_MODE_BACK;
		case CullMode::Front: return D3D12_CULL_MODE_FRONT;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_cull_mode"); return D3D12_CULL_MODE_NONE;
	}
}*/

D3D12_COMPARISON_FUNC GFXToD3D12ComparisonFunc(const ComparisonFunc func) {
	
	switch(func) {
		case ComparisonFunc::kAlways: return D3D12_COMPARISON_FUNC_ALWAYS;
		case ComparisonFunc::kEqual: return D3D12_COMPARISON_FUNC_EQUAL;
		case ComparisonFunc::kGreater: return D3D12_COMPARISON_FUNC_GREATER;
		case ComparisonFunc::kGreaterEqual: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case ComparisonFunc::kLess: return D3D12_COMPARISON_FUNC_LESS;
		case ComparisonFunc::kLessEqual: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case ComparisonFunc::kNever: return D3D12_COMPARISON_FUNC_NEVER;
		case ComparisonFunc::kNotEqual: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		default: assert(false && "passed invalid argument to GFXToD3D12ComparisonFunc"); return D3D12_COMPARISON_FUNC_ALWAYS;
	}
}

/*
D3D12_STENCIL_OP gfx_to_d3d12_stencil_op(const StencilOp stencil_op) {
	
	switch(stencil_op) {
		case StencilOp::Zero: return D3D12_STENCIL_OP_ZERO;
		case StencilOp::Replace: return D3D12_STENCIL_OP_REPLACE;
		case StencilOp::Keep: return D3D12_STENCIL_OP_KEEP;
		case StencilOp::Invert: return D3D12_STENCIL_OP_INVERT;
		case StencilOp::IncrSat: return D3D12_STENCIL_OP_INCR_SAT;
		case StencilOp::Incr: return D3D12_STENCIL_OP_INCR;
		case StencilOp::DecrSat: return D3D12_STENCIL_OP_DECR_SAT;
		case StencilOp::Decr: return D3D12_STENCIL_OP_DECR;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_stencil_op"); return D3D12_STENCIL_OP_ZERO;
	}
}

D3D12_BLEND gfx_to_d3d12_blend(const Blend blend) {
	
	switch(blend) {
		case Blend::Zero: return D3D12_BLEND_ZERO;
		case Blend::One: return D3D12_BLEND_ONE;
		case Blend::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
		case Blend::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_blend"); return D3D12_BLEND_ZERO;
	}
}

D3D12_BLEND_OP gfx_to_d3d12_blend_op(const BlendOp blend_op) {
	
	switch(blend_op) {
		case BlendOp::Add: return D3D12_BLEND_OP_ADD;
		case BlendOp::Min: return D3D12_BLEND_OP_MIN;
		case BlendOp::Max: return D3D12_BLEND_OP_MAX;
		case BlendOp::RevSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
		case BlendOp::Subtract: return D3D12_BLEND_OP_SUBTRACT;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_blend_op"); return static_cast<D3D12_BLEND_OP>(0);
	}
}

D3D12_SHADER_VISIBILITY gfx_to_d3d12_shader_visibility(const ShaderType shader_type) {
	
	switch(shader_type) {
		case ShaderType::Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
		case ShaderType::Pixel: return D3D12_SHADER_VISIBILITY_PIXEL;
		case ShaderType::Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
		case ShaderType::Hull: return D3D12_SHADER_VISIBILITY_HULL;
		case ShaderType::Domain: return D3D12_SHADER_VISIBILITY_DOMAIN;
		case ShaderType::All: return D3D12_SHADER_VISIBILITY_ALL;
		case ShaderType::Compute: return D3D12_SHADER_VISIBILITY_ALL;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_shader_visibility"); return D3D12_SHADER_VISIBILITY_ALL;
	}
}

D3D12_DESCRIPTOR_RANGE_TYPE gfx_to_d3d12_descriptor_range_type(const ViewType view_type) {
	
	switch(view_type) {
		case ViewType::Texture:
		case ViewType::StructuredBuffer:
		case ViewType::Buffer: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		case ViewType::RWTexture:
		case ViewType::RWBuffer:
		case ViewType::RWStructuredBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		case ViewType::ConstantBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		case ViewType::Sampler: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_descriptor_range_type"); return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	}
}*/

D3D12_COMMAND_LIST_TYPE GFXToD3D12CommandListType(const CommandBufferType type) {
	
	switch(type) {
        case CommandBufferType::kGraphics: return D3D12_COMMAND_LIST_TYPE_DIRECT;
        case CommandBufferType::kCopy: return D3D12_COMMAND_LIST_TYPE_COPY;
        case CommandBufferType::kCompute: return D3D12_COMMAND_LIST_TYPE_COMPUTE;
		default: assert(false && "passed invalid argument to GFXToD3D12CommandListType"); return D3D12_COMMAND_LIST_TYPE_DIRECT;
    }
}

D3D12_HEAP_TYPE GFXToD3D12HeapType(const MemoryType memory_type) {
	
	switch(memory_type) {
        case MemoryType::kDefault: return D3D12_HEAP_TYPE_DEFAULT;
        case MemoryType::kUpload: return D3D12_HEAP_TYPE_UPLOAD;
        case MemoryType::kReadBack: return D3D12_HEAP_TYPE_READBACK;
		default: assert(false && "passed invalid argument to GFXToD3D12HeapType"); return D3D12_HEAP_TYPE_DEFAULT;
    }
}

D3D12_RESOURCE_STATES GFXToD3D12ResourceState(const MemoryState state) {
	
	switch(state) {
		case MemoryState::kCommon: return D3D12_RESOURCE_STATE_COMMON;
		case MemoryState::kRenderTarget: return D3D12_RESOURCE_STATE_RENDER_TARGET;
		case MemoryState::kPresent: return D3D12_RESOURCE_STATE_PRESENT;
		default: assert(false && "passed invalid argument to GFXToD3D12ResourceState"); return D3D12_RESOURCE_STATE_COMMON;
	}
}

/*
D3D12_RESOURCE_DIMENSION gfx_to_d3d12_resource_dimension(const ViewDimension view_dimension) {
	
	switch(view_dimension) {
		case ViewDimension::Buffer: return D3D12_RESOURCE_DIMENSION_BUFFER;
		case ViewDimension::Texture1D: return D3D12_RESOURCE_DIMENSION_TEXTURE1D;
		case ViewDimension::Texture2D: return D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		case ViewDimension::Texture3D: return D3D12_RESOURCE_DIMENSION_TEXTURE3D;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_resource_dimension"); return D3D12_RESOURCE_DIMENSION_UNKNOWN;
	}
}
*/

D3D12_FILTER GFXToD3D12Filter(const Filter filter) {
	
	switch(filter) {
		case Filter::kAnisotropic: return D3D12_FILTER_ANISOTROPIC;
		case Filter::kComparisonMinMagMipLinear: return D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		case Filter::kMinMagMipLinear: return D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		default: assert(false && "passed invalid argument to GFXToD3D12Filter"); return D3D12_FILTER_ANISOTROPIC;
	}
}

D3D12_TEXTURE_ADDRESS_MODE GFXToD3D12TextureAddressMode(const TextureAddressMode mode) {
	
	switch(mode) {
		case TextureAddressMode::kClamp: return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
		case TextureAddressMode::kWrap: return D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		default: assert(false && "passed invalid argument to GFXToD3D12TextureAddressMode"); return D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
	}
}

/*
D3D12_DESCRIPTOR_HEAP_TYPE gfx_to_d3d12_descriptor_heap_type(const ViewType view_type) {
	
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
		default: assert(false && "passed invalid argument to gfx_to_d3d12_descriptor_heap_type"); return D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	}
}*/

D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE GFXToD3D12RenderPassBeginType(const RenderPassLoadOp op) {
	
	switch (op) {
    	case RenderPassLoadOp::kLoad: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_PRESERVE;
    	case RenderPassLoadOp::kClear: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_CLEAR;
		case RenderPassLoadOp::kDontCare: return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
		default: assert(false && "passed invalid argument to gfx_to_d3d12_render_pass_begin_type"); return D3D12_RENDER_PASS_BEGINNING_ACCESS_TYPE_DISCARD;
    }
}

D3D12_RENDER_PASS_ENDING_ACCESS_TYPE GFXToD3D12RenderPassEndType(const RenderPassStoreOp op) {
    
	switch (op) {
    	case RenderPassStoreOp::kStore: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_PRESERVE;
    	case RenderPassStoreOp::kDontCare: return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
		default: assert(false && "passed invalid argument to GFXToD3D12RenderPassEndType"); return D3D12_RENDER_PASS_ENDING_ACCESS_TYPE_DISCARD;
    }
}

}
